# -*- coding: utf-8 -*-
"""
Created on Mon Nov 14 15:39:18 2022

@author: MTE
"""

# import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks
import time

debug = True

from Evaluation import Evaluation_Basics as ev_b
from Evaluation import Bildfun


# import grid design 
[grid_height, grid_width], look_up = Bildfun.grid_params()

# mask & cam parameter
h1  = 800
h2  = 20
px_size         = 3.45
stripe_width    = 100

def calc_d_k(lines):                
    if lines[1,0] == lines[0,0]:
        line_0 = np.mean(lines[:2,1])
    else:
        line_0 = lines[0,1]
    if lines[-1,0] == lines[-2,0]:
        line_n = np.mean(lines[-2:,1])
    else:
        line_n = lines[-1,1]    
    
    return (line_n-line_0)/((lines[-1,0] - lines[0,0])/200)

def check_grids(grids):    
    for row in np.arange(np.size(grids,0)):
        for col in np.arange(np.size(grids,1)):
            # print("check_grids values:")
            # print(row,col, len(grids[row,col].max_pos))
            while (ev_b.decumulate(grids[row,col].max_pos) > 100).any():
                for i_fill in np.flip(np.where(ev_b.decumulate\
                                        (grids[row,col].max_pos) > 100)[0]):
                    grids[row,col].max_pos = np.insert(grids[row,col].max_pos ,i_fill+1, \
                                                np.mean(grids[row,col].max_pos[i_fill:i_fill+2]))
            
            if len(grids[row,col].max_pos) >= 1:
                if ((row != np.size(grids,0)-1 and grids[row,col].orientation == 'hor' and 
                     grids[row+1,col].im_loc[0]-grids[row,col].max_pos[-1] > 85) or
                     (col != np.size(grids,1)-1 and grids[row,col].orientation == 'ver' and 
                      grids[row,col+1].im_loc[1]-grids[row,col].max_pos[-1] > 115)):
                    new_mp = np.mean(ev_b.decumulate(grids[row,col].max_pos)) + \
                                     grids[row,col].max_pos[-1]
                    grids[row,col].max_pos = np.append(grids[row,col].max_pos, new_mp)
                    
                elif ((row != 0 and grids[row,col].orientation == 'hor' and
                      grids[row,col].max_pos[0]-grids[row,col].im_loc[0] > 85) or
                      (col != 0 and grids[row,col].orientation == 'ver' and
                       grids[row,col].max_pos[0]-grids[row,col].im_loc[1] > 85)):
                    new_mp = - np.mean(ev_b.decumulate(grids[row,col].max_pos)) + \
                                        grids[row,col].max_pos[0]
                    grids[row,col].max_pos = np.insert(grids[row,col].max_pos, 0, new_mp)

    return grids

def cut_grid(grid_im):
    # start       = time.time()
    grid_im2    = grid_im[::2,::2]
    mean_row    = np.mean(grid_im2,1)
    im_mean     = np.mean(mean_row)
    val_range   = np.amax(grid_im2)-np.amin(grid_im2)
    where_out   = np.where([((mean_row[i]<im_mean-0.075*val_range \
                            or mean_row[i]>=im_mean+0.075*val_range) \
                            and (i<len(mean_row)/4 or i>=len(mean_row)*3/4))
                            for i in np.arange(len(mean_row))])[0]
    
    if len (where_out) >= 1:
        where_out   = np.insert(where_out,0,0)
        where_out   = np.append(where_out,len(mean_row)-1)
        where_arg   = np.argmax(ev_b.decumulate(where_out))
        grid_cut    = grid_im[where_out[where_arg]*2:where_out[where_arg+1]*2,:]

    else:
        grid_cut    = grid_im
    # stop        = time.time()
    # print(stop-start)
    return grid_cut

def get_d_k(grids,grid_width,grid_height):
    lines_hor = []
    lines_ver = []
    for row in np.arange(np.size(grids,0)):
        for col in np.arange(np.size(grids,1)):
            field = grids[row,col]
            # print(field.orientation, field.max_pos)
            if len(field.max_pos) >= 1:
                if field.orientation == 'hor':
                    lines_hor.append([get_mask_pos(field,row,col,0,grid_width,grid_height),\
                                     field.max_pos[0]])
                    lines_hor.append([get_mask_pos(field,row,col,len(field.max_pos)-1,\
                                                  grid_width,grid_height),\
                                     field.max_pos[-1]])
                else:
                    lines_ver.append([get_mask_pos(field,row,col,0,grid_width,grid_height),\
                                     field.max_pos[0]])
                    lines_ver.append([get_mask_pos(field,row,col,len(field.max_pos)-1,\
                                                  grid_width,grid_height),\
                                     field.max_pos[-1]])
    
    if len(lines_ver) >= 3 and len(lines_hor)>=3:
        d_k_hor = calc_d_k(np.sort(np.array(lines_hor),0))
        d_k_ver = calc_d_k(np.sort(np.array(lines_ver),0))
        d_k_mean = np.mean(np.array([d_k_hor,d_k_ver]))
    else:
        d_k_mean = 200/px_size
    return d_k_mean

def get_mask_pos(field,row,col,i_max,grid_width,grid_height):
    if field.orientation == 'hor':
        if row == 0:
            s_index = i_max + 6 - len(field.max_pos)
        else: 
            s_index = i_max
        mask_pos = s_index* 200 + 350 + (row-1)*grid_height
    else:
        if col == 0:
            s_index = i_max + 8 - len(field.max_pos[:9])
        else: 
            s_index = i_max
        mask_pos = s_index* 200 + 350 + (col-1)*grid_width
    return mask_pos

def grid_pos(image, mode='gauss'):

    if debug:
        print("image:" + str(image))
        print("len image: " + str(np.size(image,0)) +", " + str(np.size(image,1)))
    image2  = np.copy(image[::2,::2])

    if debug:
        print("image2:" + str(image2))
        print("len image2: " + str(np.size(image2, 0)) + ", " + str(np.size(image2, 1)))
    # start = time.time()
    # grad    = np.gradient(image2)
    # dur = time.time()-start
    # print(dur)
    
    cut_hor, cut_ver = Bildfun.find_edges(image2)
    if debug:
        print("Cut hor:" + str(cut_hor))
        print("Cut ver:" + str(cut_ver))

    if len (cut_ver) >= 2 and len (cut_hor) >= 2:
        if cut_ver[0]*2 < 10:
            cut_ver = np.delete(cut_ver,0)
            
        if cut_hor[0]*2 < 10:
            cut_hor = np.delete(cut_hor,0)
        
        cut_ver = np.insert(cut_ver, 0, 0)
        cut_hor = np.insert(cut_hor, 0, 0)
        
        cut_ver = np.append(cut_ver, int(np.size(image,1)/2))
        cut_hor = np.append(cut_hor, int(np.size(image,0)/2))
        
        cut_ver = cut_ver.astype(int)
        cut_hor = cut_hor.astype(int)

        if debug:
            print("Cut hor1:" + str(cut_hor))
            print("Cut ver1:" + str(cut_ver))

        time_0 = time.time()
        
        grids   = np.empty((len(cut_hor)-1,len(cut_ver)-1), dtype = object)
        d_mean  = []
        
        five_percent = np.size(image) * 0.05
        for row in np.arange(len(cut_hor)-1):
            for col in np.arange(len(cut_ver)-1):       
                # breakpoint()
                grid0       = image[int(cut_hor[row]*2):int(cut_hor[row+1]*2),
                                    int(cut_ver[col]*2):int(cut_ver[col+1]*2)]
                # if debug:
                #     print("grid0: " + str(grid0))

                # grad_grid0  = grad[0][cut_hor[row]:cut_hor[row+1],cut_ver[col]:cut_ver[col+1]]
                # grad_grid1  = grad[1][cut_hor[row]:cut_hor[row+1],cut_ver[col]:cut_ver[col+1]]
                
                mean2_0     = np.mean(image2[cut_hor[row]:cut_hor[row+1],cut_ver[col]:cut_ver[col+1]],0)
                mean2_1     = np.mean(image2[cut_hor[row]:cut_hor[row+1],cut_ver[col]:cut_ver[col+1]],1)
                mean_grad0  = np.mean(np.abs(np.gradient(ev_b.bandfilter(mean2_0,[0,int(len(mean2_0)/6)]))))#np.mean(np.abs(grad_grid0))
                mean_grad1  = np.mean(np.abs(np.gradient(ev_b.bandfilter(mean2_1,[0,int(len(mean2_1)/6)]))))#np.mean(np.abs(grad_grid1))
                # if debug:
                #     print("mean_grad0: " + str(mean_grad0))
                #     print("mean_grad1: " + str(mean_grad1))
                if mean_grad1 > mean_grad0:
                    orientation = 'hor'
                else:
                    orientation = 'ver'
                    
                if orientation == 'hor':
                    grid_rot = np.flip(np.rot90(grid0),0)
                else:
                    grid_rot = grid0
                
                # if row == 1 and col == 1: breakpoint()
                # start = time.time()
                if (np.size(grid_rot) >= five_percent or \
                    (orientation == 'ver' and row == 0 and col == 1) or\
                    (orientation == 'hor' and row == 1 and col == 0)):
                    
                    grid_cut        = cut_grid(grid_rot)
                    max_pos, pres   = ev_b.subpx_max_pos(grid_cut, stripe_width, px_size/1000, mode)
                    # if debug:
                        # print("grid_cut: " + str(grid_cut))
                        # print("max_pos: " + str(max_pos))
                        # print("row: " + str(row) + " ,col: " + str(col))
                    if len(max_pos) > 1 and ev_b.decumulate(max_pos)[-1] > 65:
                        max_pos = max_pos[:-1]
                    if len(max_pos) > 1 and ev_b.decumulate(max_pos)[0] > 65:
                        max_pos = max_pos[1:]
                else:
                    max_pos = pres  = []
    
                grids[row,col]  = Bildfun.grid(grid_rot, orientation, [cut_hor[row]*2, cut_ver[col]*2], max_pos)

        time_1 = time.time()
        
        # im_0 = []
        # for row in np.arange(np.size(grids,0)):
        #     for col in np.arange(np.size(grids,1)):
        #         im_0.append([row,col,grids[row,col].max_pos])
        #         # print(len(grids[row,col].max_pos))
        
        # print(count)
        # if count == 250: breakpoint()
        grids = check_grids(grids)

        index, ind_ori = Bildfun.read_binary(grids,image)
        if debug:
            print("index: " + str(index))
            print("ind_ori: " + str(ind_ori))
            # for a in np.arange(0,3):
            #     for b in np.arange(0,3):
            #         print("original grids: " + str(grids[a,b].orientation) + ", " + str(a) + " ," + str(b))
        time_2 = time.time()
        # find k per image
        # d_mean = []
        for row in np.arange(np.size(grids,0)):
            for col in np.arange(np.size(grids,1)):
                field = grids[row,col]
                # print(field.max_pos)
                if ((field.orientation == 'hor' and
                        (len(field.max_pos) == 7 or row == np.size(grids,0)-1)) or
                    (field.orientation == 'ver' and 
                        (len(field.max_pos) >= 9 or col == np.size(grids,1)-1))):
                    field.max_pos = field.max_pos[1:]
                    # if debug:
                    #     print("field grids:" + str(field.orientation)+ ", " + str(row) + " ," + str(col))
                    #     print("grids:" + str(grids[row,col].orientation) + ", " + str(row) + " ," + str(col))

                    # lösche 0. Streifen auch am rechten/ unteren Rand - delete 0. stripe also at right/bottom edge
                # d_mean_0 = np.mean(ev_b.decumulate(field.max_pos[:9]))
                # d_mean.append([d_mean_0, field.px_num])
        
        d_k = get_d_k(grids, grid_width,grid_height) #Bildfun.weighted_average(d_mean)
        k   = d_k * px_size/200
        if debug:
            print("k: " + str(k))
        #     print("dk grids: " + str(grids[row,col].max_pos))

        time_3 = time.time()
        center_hor = []
        center_ver = []
        if index >= 0 and index < 200:
            if grids[1,1].orientation == 'hor':
                look_el = look_up[index,0]
            else:
                look_el = look_up[index,1] 
        else:
            look_el = [0,0]

        for row in np.arange(np.size(grids,0)):
            for col in np.arange(np.size(grids,1)):
                field = grids[row,col]
                center = []
                for i_max in np.arange(len(field.max_pos[:8])):
                    mask_pos = get_mask_pos(field,row,col,i_max,grid_width,grid_height)
                    if field.orientation == 'hor':
                        P   = mask_pos + look_el[0]
                    else:
                        P   = mask_pos + look_el[1]
                    center.append(-P*k + field.max_pos[i_max]*px_size)
                if field.orientation == 'hor':
                    center_hor.append([np.mean(np.array(center)),field.px_num])
                    # print('hor', len(center))
                else:
                    center_ver.append([np.mean(np.array(center)),field.px_num])
                    # print('ver', len(center))
        times = [time_0, time_1, time_2, time_3]
        if debug:
            print("times: " + str(times))
    
    else:
        center_ver  = [[],[]],[[],[]]   
        center_hor  = [[],[]],[[],[]] 
        index       = np.nan
        k           = np.nan
        ind_ori     = np.nan 
        grids       = []
        times       = []
    
    return Bildfun.weighted_average(center_ver), Bildfun.weighted_average(center_hor), index, ind_ori, k, grids, times
