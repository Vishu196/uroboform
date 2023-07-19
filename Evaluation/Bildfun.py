# -*- coding: utf-8 -*-
"""
Created on Tue Nov  8 08:10:39 2022

@author: MTE
"""

# import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks

from . import Evaluation_Basics as ev_b

class grid:
    def __init__(self, image, orientation, coord, max_pos):
        self.image = image
        self.px_num = np.size(image)
        self.orientation = orientation
        self.im_loc = coord
        
        if orientation == 'hor':
            self.max_pos = coord[0] + max_pos
        else: 
            self.max_pos = coord[1] + max_pos
            
def grid_params():
    
    grid_height = 1500 # mikro meter
    grid_width  = 2000 # mikro meter
    
    look_up = np.empty((200,2), dtype = object)
    for i in np.arange(len(look_up)):
        # 0: hor, 1: ver
        col     = i % 10   
        d_col = (col-5)*2*grid_width
        
        row     = int(i/10)
        d_row   = (row-10)*grid_height
        if int(i/10)% 2 == 0:
            look_up[i,0] = [d_row, d_col]
            look_up[i,1] = [d_row, d_col+grid_width]    
        else:
            look_up[i,0] = [d_row, d_col+grid_width]   
            look_up[i,1] = [d_row, d_col]   
            
    return [grid_height, grid_width], look_up

def read_binary(grids,image, plot = False):
    if len(grids[1,1].max_pos) >= 5:
        ori         = grids[1,1].orientation
        if ori == 'hor':
            if len(grids[0,1].max_pos) == 9:
                max_mean   = grids[0,1].max_pos
            elif len(grids[2,1].max_pos) == 9:
                max_mean   = grids[2,1].max_pos
            else: max_mean = []
        else:
            if len(grids[1,0].max_pos) == 7:
                max_mean   = grids[1,0].max_pos
            elif len(grids[1,2].max_pos) == 7:
                max_mean   = grids[1,2].max_pos
            else: max_mean = []
        
        d_mean      = np.mean(ev_b.decumulate(max_mean))
        
        if len(max_mean) >= 4 and d_mean >= 50 and d_mean <= 70:
            bounds      = np.linspace(max_mean[0]-d_mean*3/4,max_mean[3]+d_mean*3/4,10).astype(int)
            
            if ori == 'hor':
                coded_area = image[int(grids[1,1].max_pos[0]-d_mean/4):\
                                   int(grids[1,1].max_pos[0]+d_mean/4),:]
                coded_line = np.mean(coded_area,0)
                # coded_line = image[int(grids[1,1].max_pos[0]),:]
            else:
                coded_area = image[:,int(grids[1,1].max_pos[0]-d_mean/4):\
                                     int(grids[1,1].max_pos[0]+d_mean/4)]
                coded_line = np.mean(coded_area,1)
                
            code_mean = [np.mean(coded_line[bounds[i]:bounds[i+1]]) \
                     for i in np.arange(9)] 
            th = np.amin(image)+(np.mean(image)-np.amin(image))*0.85 #np.mean(image)
            code_bin = (code_mean < th).astype(np.int_)
            bin_str = "".join(code_bin[1:].astype(str))
            code = int(bin_str,2)

        else:
            code = 300
    else:
        code = 400
        ori  = 'non'

    if plot:
        # fw = 10
        # array = image[cut_hor[1]-fw:cut_hor[2]+ fw, cut_ver[1]-fw:cut_ver[2]+ fw]
        fig = plt.figure(figsize=[3.0, 1.8], dpi = 300)
        show_0 = plt.imshow(image, cmap='gray')# \
                            #extent=(cut_ver[1]-fw,cut_ver[2]+ fw,cut_hor[2]+ fw,cut_hor[1]-fw))
        # if ori == 'hor':
            # plt.vlines(bounds - (cut_ver[1]-fw), 0, np.size(array,0), 'r', lw = 0.5)
        # else:
        #     plt.hlines(bounds, 0, np.size(array,1), 'r', lw = 0.8)
        # plt.hlines(int(grids[1,1].max_pos[0])-(cut_hor[1]-fw), 0, np.size(array,1), 'navy', lw = 0.8)
        
        plt.xlabel(r"position $\xi$ in px")
        plt.ylabel(r"position $\zeta$ in px")
        cb = fig.colorbar(show_0, fraction=0.0366)
        cb.set_label('intensity')
        plt.show()
        
        intensity = image[int(grids[1,1].max_pos[0]),:]
        # th = np.mean(image)
        plt.subplots(dpi = 300, figsize = [3.0,1.8])
        plt.plot(coded_line,\
                 'navy', lw = 0.75)
        plt.vlines(bounds, np.amin(intensity), np.amax(intensity), 'r', lw = 0.5)
        plt.hlines(th, 0, len(intensity), 'orange', lw = 0.5)
        plt.xlabel(r"position $\xi$ in px")
        plt.ylabel(r"intensity")
        plt.show()

    return code, ori


def weighted_average(array):
    valid_vals = []
    for val_0 in array:
        if not np.isnan(val_0).any():
            valid_vals.append(val_0)
    valid_vals = np.array(valid_vals)
    # breakpoint()
    try:
        av_val = np.sum(valid_vals[:,0]*valid_vals[:,1])/np.sum(valid_vals[:,1])
    except IndexError:
        av_val = np.nan
    return av_val


def line_index(mean_range_in, th_edge, i0, rank = 1):
    mean_range = ev_b.bandfilter(mean_range_in, [0,int(len(mean_range_in)/6)])
    stripes, s_dic = find_peaks(mean_range, distance = 50/2, height = th_edge)
    stripes_min, s_dic_min = find_peaks(-mean_range, distance = 50/2, height = -th_edge)
    
    if len(stripes) >= 1 and rank <= len(s_dic['peak_heights']):
        s_max = stripes[np.argsort(s_dic['peak_heights'])[-rank]] + i0
    else:
        s_max = np.nan
    if len(stripes) >= 3 and len(stripes_min) >= 3:
        if stripes_min[0] > 25:
            stripes_min = np.insert(stripes_min, 0, 0)
            s_dic_min['peak_heights'] = np.insert(s_dic_min['peak_heights'], 0, -mean_range[0])
        if (len(mean_range)-stripes_min[len(stripes_min)-1]) > 25: 
            stripes_min = np.append(stripes_min, len(mean_range)-1)
            s_dic_min['peak_heights'] = np.append(s_dic_min['peak_heights'],-mean_range[len(mean_range)-1])
        s_min = stripes_min[np.argmax(s_dic_min['peak_heights'][1:len(stripes_min)-1])+1] + i0
    else: 
        s_min = np.nan
    return s_max, s_min

def delete_edges(cut_ver, ideal_d):
    for i_cut in np.arange(len(cut_ver)-1,-1,-1):
        if not np.array([np.abs(cut_ver[j]-cut_ver[i_cut]) > ideal_d-40 
             and np.abs(cut_ver[j]-cut_ver[i_cut]) < ideal_d+40
             for j in np.arange(len(cut_ver))]).any():
            cut_ver = np.delete(cut_ver,i_cut)
    
    # (ev_b.decumulate(cut_ver) < 280).any() # lösche den richtigen Wert, wenn zu viele Kanten detektiert wurden!
    close_edges = np.where(ev_b.decumulate(cut_ver) < ideal_d-20)[0]
    for i_close in np.arange(len(close_edges)-1,-1,-1):
        d_cut_ver_0 = []
        d_cut_ver_1 = []
        if close_edges[i_close] > 0:
            d_cut_ver_0.append(np.abs(cut_ver[close_edges[i_close]]\
                                      -cut_ver[close_edges[i_close]-1]))
            d_cut_ver_1.append(np.abs(cut_ver[close_edges[i_close]+1]\
                                      -cut_ver[close_edges[i_close]-1]))
        if close_edges[i_close] < len(cut_ver)-2:
            d_cut_ver_0.append(np.abs(cut_ver[close_edges[i_close]]\
                                      -cut_ver[close_edges[i_close]+2]))
            d_cut_ver_1.append(np.abs(cut_ver[close_edges[i_close]+1]\
                                      -cut_ver[close_edges[i_close]+2]))
        d_m_0 = np.abs(np.mean(np.array(d_cut_ver_0))-ideal_d)
        d_m_1 = np.abs(np.mean(np.array(d_cut_ver_1))-ideal_d)
        
        if d_m_0 > d_m_1:
            #delete element i_close
            cut_ver = np.delete(cut_ver, close_edges[i_close])
        else:
            #delete element i_close+1
            cut_ver = np.delete(cut_ver, close_edges[i_close+1])
        close_edges = np.where(ev_b.decumulate(cut_ver) < ideal_d-20)[0]
    return cut_ver

def detect_through(im_col, th_edge):
    im_diff = im_col - th_edge
    th_through = [np.sign(im_diff[i])!= np.sign(im_diff[i+1]) for i in np.arange(len(im_diff)-1)]
    through_loc = np.where(th_through)[0]
    through_loc = np.insert(through_loc,0,0)
    through_loc = np.append(through_loc, len(im_diff))
    
    d_through = ev_b.decumulate(through_loc)
    cut_through = np.where(d_through > 33)[0]
    return through_loc, cut_through

def find_edges(image2):
    mean0 = np.mean(image2,0)
    mean1 = np.mean(image2,1)
    
    freq_range = 150
    main_d_0 = np.median([1/ev_b.main_freq(mean0[i:i+freq_range])[2] for i in np.arange(0,len(mean0)-freq_range,50)])
    main_d_1 = np.median([1/ev_b.main_freq(mean1[i:i+freq_range])[2] for i in np.arange(0,len(mean1)-freq_range,50)])
    if main_d_0 > 13 and main_d_1 > 13:
        
        th_edge = np.mean(image2)
        
        mid = int(len(mean0)/2)
        search_range = 150
        mean_range0 = mean0[mid-search_range:mid+search_range]
        i0 = mid-search_range
        
        rank = 0
        while rank < 5:
            rank += 1
            s_max = line_index(mean_range0, th_edge, i0, rank) [0]
            if not np.isnan(s_max):
               
                im_col = ev_b.bandfilter(image2[:,s_max],[0,int(np.size(image2,0)/6)])
                    
                std_col = [np.std(im_col[i:i+150]) for i in np.arange(len(im_col)-150)]
                if np.amin(std_col)/(np.amax(im_col)-np.amin(im_col)) <= 0.085:
                    break
        
        try:
            cut_hor = []
            if rank != 5:
                through_loc, cut_through = detect_through(im_col, th_edge)
        
                for i in np.arange(len(cut_through)):
                    if through_loc[cut_through[i]] == 0 and im_col[through_loc[cut_through[i]]+1] > th_edge:
                        cut_hor.append(through_loc[cut_through[i]+1])
                    elif cut_through[i] == len(through_loc)-2:
                        cut_hor.append(through_loc[cut_through[i]])
                    elif im_col[through_loc[cut_through[i]]+1] > th_edge:
                        if through_loc[cut_through[i]] > 10:
                            cut_hor.append(through_loc[cut_through[i]])
                        cut_hor.append(through_loc[cut_through[i]+1])
                cut_hor = np.array(cut_hor)
                
                cut_hor = delete_edges(cut_hor,225)
            # # teste ob ein Wert doppelt vorhanden ist
            # hor_twice = np.where(cut_hor[1:] == cut_hor[:-1])[0]
            # for i_twice in np.arange(len(hor_twice)-1,-1,-1):
            #     cut_hor = np.delete(cut_hor,hor_twice[i_twice])
        
        except IndexError:
            cut_hor = []
            
        if len(cut_hor) >= 2:
            mean_range1 = mean1[cut_hor[0]:cut_hor[1]]
            s_max,s_min = line_index(mean_range1, th_edge, cut_hor[0])
            
            # linke Kanten
            try:
                cut_ver = []
                im_row_low = ev_b.bandfilter(image2[s_min,:],[0,int(np.size(image2,1)/6)])
                # th_row = np.mean(im_row)
    
                std_row = [np.std(im_row_low[i:i+200]) for i in np.arange(len(im_row_low)-200)]
                if np.amin(std_row)/(np.amax(im_row_low)-np.amin(im_row_low)) <= 0.085:
                    through_loc, cut_through = detect_through(im_row_low, th_edge)
        
                    for i_row_low in np.arange(len(cut_through)):
                        if through_loc[cut_through[i_row_low]] != 0:
                            cut_ver.append(through_loc[cut_through[i_row_low]])
                
                #rechte Kanten
                im_row = ev_b.bandfilter(image2[s_max,:],[0,int(np.size(image2,1)/6)])
                std_row = [np.std(im_row[i:i+200]) for i in np.arange(len(im_row)-200)]
                if np.amin(std_row)/(np.amax(im_row)-np.amin(im_row)) <= 0.088:
                    through_loc, cut_through = detect_through(im_row, th_edge)
                    for i_row_high in np.arange(len(cut_through)):
                        if through_loc[cut_through[i_row_high]+1] != len(im_row):
                            cut_ver.append(through_loc[cut_through[i_row_high]+1])
                
                cut_ver = np.sort(cut_ver)
                
                # in jedem 280 px Bereich darf maximal ein cut liegen!
                cut_ver = delete_edges(cut_ver,300)
                    
            except IndexError:
                cut_ver = []
        
        else:
            cut_ver = []
            
    else:
        cut_hor = cut_ver = []
    
    return cut_hor, cut_ver