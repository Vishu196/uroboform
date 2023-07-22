# -*- coding: utf-8 -*-
"""
Created on Thu May 19 08:32:07 2022

@author: MTE
"""

import copy
import cmath
import math
import matplotlib.pyplot as plt
import numpy as np
#from outliers import smirnov_grubbs as grubbs
from scipy import signal
from scipy.fftpack import rfft, irfft
from scipy.optimize import curve_fit
from scipy.stats import linregress

#%%
def bandfilter(x, limits):
    [x0,x1] = limits
    f_x     = rfft(x)
    f_x_cut = copy.copy(f_x)
    f_x_cut[:x0] = 0
    f_x_cut[x1:] = 0
    x_cut   = irfft(f_x_cut)
    return x_cut

def decumulate(x):
    if len(x) <= 1:
        xi = np.array([])
    else:
        xi = x[1:]-x[:-1]
    return xi

def gauss_limited(x, k, sigma, mu, offset, max_cut):
    y = k*math.e**(-(x-mu)**2/(2*sigma**2)) + offset
    for i in np.arange(len(y)):
        if y[i] >= max_cut: y[i] = max_cut
    return y

def main_freq(B):
    B = B - np.mean(B)
    wfun = signal.windows.blackman(len(B))
    image_window = B * wfun 
    
    # Anwendung der FFT
    y   = np.fft.fft(image_window)#, norm = 'forward')
    
    n_g = spek_interpol1(abs(y))
    f_g = n_g / len(B)
    return image_window, n_g, f_g

def spek_interpol1(A, lower_half = True):
    # Spektrale Interpolation
    A_size      = len(A)
    if lower_half:
        n_0         = np.argmax (A[0:int(A_size/2)])
    else:
        n_0         = np.argmax (A)
    y_ln1       = np.log(A[n_0+1])
    y_ln0       = np.log(A[n_0])
    y_ln_1      = np.log(A[n_0-1])
    n_g         = n_0 + 1/2 * (y_ln_1-y_ln1)/(y_ln_1 - 2* y_ln0 + y_ln1)
    return n_g

def subpx_gauss(B_cut,B_max,B_min,d_m):
    max_pos = []
    popt_res = []
    for i_b in B_max:
        mid     = int(i_b)
        
        if len(B_min) >=2:
            for i_0 in B_min:
                if i_0 < mid: xmin = i_0
            for i_1 in B_min[::-1]:
                if i_1 > mid: xmax = i_1
            
            
            try: 
                if not xmin == xmax:
                    popt,pcov = curve_fit(gauss_limited, np.arange(xmin,xmax), B_cut[xmin:xmax], \
                                              p0 = [B_cut[mid]-B_cut[xmin],\
                                                    0.16*d_m,mid,B_cut[xmin],\
                                                    0.95*(B_cut[mid]-B_cut[xmin])+B_cut[xmin]])
                    popt_res.append(popt)
                    max_pos.append(popt[2])
                    
                    # plt.subplots(figsize=[4.0, 2.5], dpi = 300)
                    # # plt.plot(np.arange(len(B_cut)), B_cut)
                    # plt.plot(np.arange(xmin,xmax),B_cut[xmin:xmax], color = 'deepskyblue', 
                    #          lw = 1.2, label = 'filtered intensity')
                    # plt.plot(np.arange(xmin,xmax),gauss_limited(np.arange(xmin,xmax),\
                    #         popt[0],popt[1],popt[2],popt[3],popt[4]), color = 'deeppink', 
                    #          lw=1.2, label = 'gauss fit')
                    # plt.xlabel(r"position $\xi$ in px")
                    # plt.ylabel('intensity')
                    # plt.legend()
                    # plt.show()
            except RuntimeError:
                print('Optimal Parameters not found for image at x =', mid)
                # breakpoint()
            except NameError:
                pass
        
        try:
            del(xmin,xmax)
        except NameError:
            pass
    return max_pos, popt_res

def subpx_parabel(B_cut, B_max, B_min, d_m):
    max_pos = []
    for i_b in B_max:
        mid     = int(i_b)        
        if len(B_min) >=2:
            for i_0 in B_min:
                if i_0 < mid: xmin = int(i_b-d_m/4)+3
            for i_1 in B_min[::-1]:
                if i_1 > mid: xmax = int(i_b+d_m/4)
            try:
                if not xmin == xmax:
                    x = np.arange(xmin, xmax)
                    Phi = np.c_[x[:,np.newaxis]**[0], x[:,np.newaxis]**[1], x[:,np.newaxis]**[2]]
                    W0 = np.identity(len(x))
                    a_dach0 = np.linalg.inv(Phi.T@W0@Phi)@Phi.T@W0@B_cut[xmin: xmax]
                    max_pos.append(-a_dach0[1]/a_dach0[2]/2)
                    
                    # x = np.arange(xmin,xmax)
                    # plt.subplots(figsize=[4.0, 2.5], dpi = 300)
                    # plt.plot(x,B_cut[xmin:xmax], color = 'deepskyblue', 
                    #          lw = 1.2, label = 'filtered intensity')
                    # plt.plot(x,a_dach0[2]*x**2+a_dach0[1]*x + a_dach0[0], color = 'deeppink', 
                    #          lw=1.2, label = 'parabola fit')
                    # plt.xlabel(r"position $\xi$ in px")
                    # plt.ylabel('intensity')
                    # plt.legend()
                    # plt.show()
            except NameError:
                pass
            
        try:
            del(xmin,xmax)
        except NameError:
            pass
        
    return max_pos, []

def subpx_max_pos(image, stripe_width, px_size, mode='gauss'):
    # breakpoint()
    if mode == 'phase':
        max_pos, popt_res = subpx_phase(image)
    
    else:
        B       = np.mean(image,0)
        filt    = 64/640*len(B)
        B_cut   = bandfilter(B, [0,int(filt)])#[0,41])
        d_min   = stripe_width/1000*2/px_size/2
        prom    = (np.amax(B_cut)-np.amin(B_cut))*0.2
        B_max, dict0 = signal.find_peaks(B_cut, distance = d_min, prominence = prom)
        B_min, dict2 = signal.find_peaks(-B_cut, distance = d_min, prominence = 0)
        
        if len(B_max) >= 1 and len(B_min)>= 1:
            if B_min[0] > B_max[0] and B_max[0] >= 0.9*d_min:
                B_min = np.insert(B_min,0,0)
            if (B_min[len(B_min)-1] < B_max[len(B_max)-1] and 
                len(B_cut) - B_max[len(B_max)-1] >= 0.9*d_min):
                B_min = np.append(B_min, len(B_cut)-1)
            if B_max[0] - B_min[0] > 0 and B_max[0] - B_min[0] < 0.8 *d_min:
                B_min = np.delete(B_min,0)
                
            d0  = decumulate(B_max)
            d_m = np.mean(d0)
            
            if mode == 'gauss':        
                max_pos, popt_res = subpx_gauss(B_cut,B_max,B_min,d_m)
            elif mode == 'parabel':        
                max_pos, popt_res = subpx_parabel(B_cut,B_max,B_min,d_m)
        
    return np.array(max_pos), popt_res


def subpx_phase(image):
    B = np.mean(image,0)
    if len (B) >= 60:
        
        image_window, n_g, f_g = main_freq(B)
        d_mean = 1/f_g
        
        # plt.subplots(figsize=[4.0, 1.5], dpi = 500)
        # plt.plot(B-np.mean(B), color = 'k', label = 'average intensity', lw = 1)
        # plt.plot(image_window, color = 'deepskyblue', label = 'filtered intensity', lw = 1)
        # plt.xlabel(r"position $\xi$ in px")
        # plt.ylabel('intensity')
        # plt.show()
        
        
        exp_fac = -1j * (2*math.pi)*np.arange(len(B))/len(B)
        
        # Phasenverschiebung
        F_k = np.sum(image_window * np.exp(exp_fac*n_g))
        Phi = -cmath.phase(F_k)
        d = Phi/(2*np.pi*f_g)
        
        A = (F_k.real**2+F_k.imag**2)**0.5
        y_cos = A * np.cos(2*math.pi*f_g*np.arange(len(image_window))-Phi)
        
        
        # plt.subplots(figsize=[4.0, 2.5], dpi = 300)
        # plt.plot(image_window, c='deepskyblue', label = 'preprocessed', lw = 1.2)
        # plt.plot(y_cos[:]/np.amax(y_cos)*(np.amax(image_window)-np.amin(image_window))/2, \
        #          c='deeppink', label = 'FFT result', lw = 1.2)
        # plt.xlabel(r"position $\xi$ in px")
        # plt.ylabel('intensity')
        # plt.legend()
        # plt.show()
        
        if d <= d_mean/2:
            d += d_mean
        
        max_pos = [d + i_max * d_mean for i_max in np.arange(n_g-1)]
    else:
        max_pos = []
    return np.array(max_pos), []
    
    