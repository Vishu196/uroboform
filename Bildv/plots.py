# -*- coding: utf-8 -*-
"""
Created on Wed Nov  9 13:24:37 2022

@author: MTE
"""

import matplotlib.pyplot as plt
plt.rcParams["text.usetex"]         = True
# plt.rcParams["font.family"]         = 'Times New Roman'
# plt.rcParams["font.serif"]          = 'computer modern'
plt.rcParams['text.latex.preamble'] = r"\usepackage{upgreek}"
plt.rcParams["font.size"]           = "14"

import numpy as np

def plot_image(image):
    fig = plt.figure(figsize=[5.3, 4.0], dpi = 300)
    show_0 = plt.imshow(image, cmap='gray', vmin = 10, vmax = 80)
    plt.xlabel(r"position $\xi$ in px")
    plt.ylabel(r"position $\zeta$ in px")
    # plt.tick_params(labelsize=12)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('intensity')
    # cb.ax.tick_params()
    # plt.title('row image')
    plt.show()
    
def plot_grad(grad):
    fig = plt.figure(figsize=[3.0, 1.8], dpi = 300)
    show_0 = plt.imshow(grad[0], cmap='gray')
    plt.xlabel(r"position $\xi$ in px")
    plt.ylabel(r"position $\zeta$ in px")
    # plt.tick_params(labelsize=12)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('gradient in 1/px')
    # cb.ax.tick_params()
    plt.title('vertical gradient')
    plt.show()
    
    fig = plt.figure(figsize=[3.0, 1.8], dpi = 300)
    show_0 = plt.imshow(grad[1], cmap='gray')
    plt.xlabel(r"position $\xi$ in px")
    plt.ylabel(r"position $\zeta$ in px")
    # plt.tick_params(labelsize=12)
    cb = fig.colorbar(show_0, fraction=0.0366)
    cb.set_label('gradient in 1/px')
    # cb.ax.tick_params()
    plt.title('horizontal gradient')
    plt.show()
    
def plot_std(grad):
    plt.subplots(figsize = [3,1.8], dpi = 300)
    plt.plot(np.std(grad[0], 0), lw = 0.75, color = 'navy')
    # plt.title('grad 0 std 0')
    plt.xlabel(r"position $\xi$ in px")
    plt.ylabel(r"std. in 1/px")
    plt.show()
    
    plt.subplots(figsize = [3,1.8],dpi = 300)
    plt.plot(np.std(grad[1],1), lw = 0.75, color = 'navy')
    plt.xlabel(r"position $\zeta$ in px")
    plt.ylabel(r"std. in 1/px")
    plt.show()

def plot_hor(xi_mean, xL, x_model, xi_sys, xi_ran):   
    fs = [4.0,3.0]
    plt.subplots(dpi = 300, figsize = fs)
    plt.plot(xL,xi_mean,'o', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    plt.plot(xL, x_model, 'k', linestyle = (0,(4,3)), lw = 1, label = 'fitted model')
    plt.xlabel(r"reference LED position $x_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"shadow position $\xi_\mathrm{i}$ in $\upmu$m")
    plt.legend()
    plt.grid()
    plt.show()
    
    # syst. error
    plt.subplots(dpi = 300, figsize = fs)
    # plt.plot(np.arctan(xL/yL), xi_sys*1000,'x', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    plt.plot(xL, xi_sys,'x', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    # plt.xlabel(r"reference angle $\Theta_\mathrm{M,ref}$ in mm")
    plt.xlabel(r"reference LED position $x_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"systematic error $\Delta(x_\mathrm{L})$ in $\upmu$m")
    # plt.legend()
    plt.grid()
    plt.show()
    
    plt.subplots(dpi = 300, figsize = fs)
    plt.plot(xL, xi_ran,'x', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    # plt.plot(xL, xL*xm + xn, 'k', linestyle = (0,(4,3)), lw = 1, label = 'fitted model')
    plt.xlabel(r"reference LED position $x_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"random error $\sigma(x_\mathrm{L})$ in $\upmu$m")
    # plt.ylim([0,16.2])
    # plt.legend()
    plt.grid()
    plt.show()
    
def plot_ver(zeta_mean, zL, z_model, zeta_sys, zeta_ran):
    fs = [4.0,3.0]
    plt.subplots(dpi = 300, figsize = fs)
    plt.plot(zL,zeta_mean,'o', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    plt.plot(zL, z_model, 'k', linestyle = (0,(4,3)), lw = 1, label = 'fitted model')
    plt.xlabel(r"reference LED position $z_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"shadow position $\zeta_\mathrm{i}$ in $\upmu$m")
    plt.legend()
    plt.grid()
    plt.show()
    
    # syst. error
    plt.subplots(dpi = 300, figsize = fs)
    plt.plot(zL, (zeta_sys)*1000,'x', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    # plt.plot(xL, xL*xm + xn, 'k', linestyle = (0,(4,3)), lw = 1, label = 'fitted model')
    plt.xlabel(r"reference LED position $z_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"systematic error $\Delta(z_\mathrm{L})$ in $\upmu$m")
    # plt.legend()
    plt.grid()
    plt.show()
    
    plt.subplots(dpi = 300, figsize = fs)
    plt.plot(zL, zeta_ran,'x', ms = 3, mfc = 'none', color =  'limegreen', label = 'measured data')
    # plt.plot(xL, xL*xm + xn, 'k', linestyle = (0,(4,3)), lw = 1, label = 'fitted model')
    plt.xlabel(r"reference LED position $z_\mathrm{L,ref}$ in mm")
    plt.ylabel(r"random error $\sigma(z_\mathrm{L})$ in $\upmu$m")
    plt.ylim([0,16.2])
    # plt.legend()
    plt.grid()
    plt.show()