# -*- coding: utf-8 -*-
"""
Created on Thu Apr 29 13:50:01 2021

@author: Marin
"""

import matplotlib.pyplot as plt
plt.rcParams["text.usetex"]         = True
plt.rcParams["font.family"]         = 'serif'
plt.rcParams["font.serif"]          = 'computer modern'
plt.rcParams["font.size"]           = "16"
plt.rcParams['text.latex.preamble'] = r"\usepackage{upgreek}"

import numpy as np
from scipy.stats import linregress
from Evaluation import Evaluation_Basics as ev_b

def image_halfslice(image, title = ''):
    # Plotten
    fig = plt.figure(figsize=[3.0, 1.8], dpi = 300)
    show_0 = plt.imshow(image, cmap='gray')
    plt.xlabel(r"position $\xi$ in px")
    plt.ylabel(r"position $\zeta$ in px")
    # plt.tick_params(labelsize=12)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('intensity')
    # cb.ax.tick_params()
    # plt.title(title)
    plt.show()
    
def lat_cali_line(step, x0_mm, pptx = True):
    if pptx:
        fs0 = [5.4,3.2]
    else:
        fs0 = [3.2, 2.0]
        
    l_pos = ev_b.cumulate(step[:])*1000
    xi_pos = ev_b.cumulate(x0_mm)
    lres = linregress(l_pos,xi_pos[:len(xi_pos)-1])
    lstp = linregress(np.arange(298),l_pos)
    scale   = np.mean(step[:])/np.mean(x0_mm)
    
    fig, ax = plt.subplots(figsize = fs0, dpi = 300)
    plt.plot(l_pos-30,xi_pos[:len(xi_pos)-1], '.', color = 'limegreen', markersize = 1, label = 'measured data')
    plt.plot(l_pos-30, l_pos/scale*1000,'k', lw = 0.8, ls = (0, (8, 8)), label = 'fitted model')
    # plt.plot(np.arange(10))
    # plt.ylabel(r"random ax. position error, $\sigma (y_L)$ (mum)")#"($\upmu$m)")
    plt.ylabel(r"shadow position, $\xi_i$ (mum)")
    plt.xlabel(r"reference lateral position, $x_{L,ref}$ (mm)")
    plt.legend()
    plt.show()
    
    fig, ax = plt.subplots(figsize = fs0, dpi = 300)
    plt.plot(l_pos,xi_pos[:len(xi_pos)-1]*scale/1000, '.', color = 'limegreen', markersize = 1, label = 'measured data')
    plt.plot(l_pos, l_pos,'k', lw = 0.8, ls = (0, (8, 8)), label = 'fitted model')
    # plt.plot(h_1,ax_fit(pos[:,1],popt[0],popt[1],popt[2])*1e3)
    # ax.invert_xaxis()
    plt.ylabel(r"measured lat. position, $x_L$ (mm)") #($\upmu$m)")
    # plt.xlabel(r"LED distance, $h_1$ (mm)")
    plt.xlabel(r"reference lat. position, $x_{L,ref}$ (mm)")
    plt.legend()
    plt.show()
    
    fig, ax = plt.subplots(figsize = fs0, dpi = 300)
    # plt.plot(l_pos,xi_pos[:len(xi_pos)-1], '.', color = 'limegreen', markersize = 1, label = 'measured data')
    plt.plot(l_pos, (xi_pos[:len(xi_pos)-1]*scale/1000-(l_pos))*1000,'.', color = 'limegreen', markersize = 1)
    # plt.plot(h_1,ax_fit(pos[:,1],popt[0],popt[1],popt[2])*1e3)
    # ax.invert_xaxis()
    plt.ylabel(r"lateral position error, $\Delta x_L$ (mum)")
    # plt.xlabel(r"LED distance, $h_1$ (mm)")
    plt.xlabel(r"reference lateral position, $x_{L,ref}$ (mm)")
    # plt.legend()
    plt.show()
    
    fig, ax = plt.subplots(figsize = fs0, dpi = 300)
    # plt.plot(l_pos,xi_pos[:len(xi_pos)-1], '.', color = 'limegreen', markersize = 1, label = 'measured data')
    plt.plot(l_pos, step-(l_pos*lstp[0]+lstp[1]),'.', color = 'limegreen', markersize = 1)
    # plt.plot(h_1,ax_fit(pos[:,1],popt[0],popt[1],popt[2])*1e3)
    # ax.invert_xaxis()
    plt.ylabel(r"shadow position, $\xi_i$ (mum)") #($\upmu$m)")
    # plt.xlabel(r"LED distance, $h_1$ (mm)")
    plt.xlabel(r"lateral position, $x_L$ (mm)")
    # plt.legend()
    plt.show()
        
    
def raw_SAS(image):
    fig = plt.figure(figsize=[6.0, 3.6], dpi = 300)
    show_0 = plt.imshow(image, cmap='gray', vmin = 0, vmax = 255)
    plt.xlabel(r"position, $\xi$ (px)")
    plt.ylabel(r"position, $\zeta$ (px)")
    # plt.tick_params(labelsize=12)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('intensity')
    # cb.ax.tick_params()
    # plt.title(title)
    plt.show()
    
def image_small(image, title = ''):
    # Plotten
    fig = plt.figure(figsize=[3.0, 1.8], dpi = 200)
    show_0 = plt.imshow(image, cmap='gray')
    plt.xlabel('position x [px]', fontsize = 11)
    plt.ylabel('position z [px]', fontsize = 11)
    plt.tick_params(labelsize=11)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('intensity [-]', fontsize = 11)
    cb.ax.tick_params(labelsize=11)
    plt.title(title, fontsize = 14)
    plt.show()

def plot_image(image, title = ''):
    # Plotten
    fig = plt.figure(figsize=[5, 2.9], dpi = 200)
    show_0 = plt.imshow(image, cmap='gray')
    plt.xlabel('position x [px]', fontsize = 12)
    plt.ylabel('position z [px]', fontsize = 12)
    plt.tick_params(labelsize=11)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('intensity [-]', fontsize = 12)
    cb.ax.tick_params(labelsize=11)
    plt.title(title, fontsize = 16)
    plt.show()
    
def plot_var(var, title = ''):
    x_v = np.arange(len(var))
    plt.plot(x_v, var)
    plt.title(title)
    plt.show()  
    
def certain_plot():
    plt.subplots(figsize=[4.0, 2.4], dpi = 300)
    plt.plot(x_LED[101:199], step[100:]*1e6/scale, linewidth = 0.8, label = 'interferometer')
    plt.plot(x_LED[101:199],x_px[0][100:198]*exp0.px_size*1000, linewidth = 0.8, \
             color = 'tab:orange', label = 'shadow sensor')
    plt.xlabel('LED position [mm]', fontsize = 12)
    plt.ylabel(r"$x_i$ $[\mu m]$",fontsize = 12)
    plt.tick_params(labelsize=12)
    # plt.title(title, fontsize = 16)
    plt.legend(fontsize= 11)
    plt.show()