# -*- coding: utf-8 -*-
"""
Created on Thu Jun 17 14:18:20 2021

@author: MTE
"""
import cv2
import glob
import math
import numpy as np
import os
import pandas

def define_experiment(folders, im_range, sens_pos, rotation = False, k=1): 
    # k = 2 --> Verwende jedes 2. Bild usw
    images_set = []
    for folder in folders:
        csv_pd = pandas.read_csv(folder+'\params.csv')
        measurement = m_set()
        measurement.add_params(csv_pd, sens_pos)
        images = read_images(folder, rotation, k)
        measurement.add_images(images[:],im_range)
        images_set.append(measurement)
    return images_set

def define_simulation(foldername, led, sens, pat, trace):
    try:
        os.mkdir(foldername)
    except FileExistsError:
        pass
    measurement = m_set()
        
    measurement.add_params_sim(led, sens, pat, trace)
    return measurement

def define_raspi(ref_pos, max_pos_list):
    meas = m_set()
    meas.path_by_pos(ref_pos)
    return meas
           
def read_images(folder, rotation = False, k=1, num = 'all'):
    paths = glob.glob(folder+'\*.bmp')
    paths.sort()
    images = []
    i_path = 0
    if num == 'all': 
        start   = 0
        end     = len(paths)
    else: 
        start   = num[0]
        end     = num[1]
    for path in paths[start:end]:
        image = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
        if i_path % 500 == 0: print('Image no.', i_path, 'imported.')
        if rotation:
            image = np.rot90(image)
        # plot_me.plot_image(image)
        if i_path % k == 0:
            images.append(image)
        i_path += 1
    return images

class m_set:
    
    def __init__(self):
        self.sensors = []
        self.results = []
        
    def add_images(self,images, im_range):
        self.images = images[im_range[0]-1:im_range[1]]
        self.extent_px = [np.size(images[0],1), np.size(images[0],0)]
        try:
            self.extent_mm = np.array(self.extent_px)*self.px_size
        except AttributeError:
            self.extent_mm = np.array([0,0])
            print('Define px size!')
        self.im_range = im_range     
        
    def add_params(self, params, sens_pos):
        # Inputs: 
        # +x zur Tür
        # +y von Licht zu Sensor
        # +z zum Boben
        self.stripe_width   = params['stripe_width'][0]
        self.px_size        = params['px_size'][0]
        self.purpose        = params['purpose'][0]
        led_step_0          = params['led_step'][0]/1000 # [mm]
        self.ang_z          = params['ausrichtung'][0]
        self.start_pos      = [[params['start_pos_1'][0]/1000,params['start_pos_2'][0]/1000,0]]
        self.no_pos         = params['no_pos'][0]
        # if self.no_pos > 182:
        #     self.no_pos = 182
        self.led_step       = np.ones(self.no_pos-1)*led_step_0
        self.sens_pos       = sens_pos#[1985,2080,0]
        # print('stimmt sens_pos:', self.sens_pos)
    
    def add_params_sim(self, led, sens, pat, trace):
        self.sens_pos       = sens.cpos[trace[10]]
        self.stripe_width   = pat.element_size[trace[2]]*1000
        self.px_size        = sens.px_size[trace[7]]
        self.ang_z          = -sens.angles[trace[9]][2]
        self.pos_list       = led
        self.no_pos         = len(led)
        
        led_step = np.zeros(len(led)-1)
        for i in np.arange(0, len(led)-1):
            led_step[i]    = np.linalg.norm(np.array(led[i+1])-np.array(led[i]))
        self.led_step = led_step
    
    def add_modes(self, modes):
        self.modes = modes
    
    def add_results(self, result):
        self.results.append(result)
        
    def add_uncertainties(self, u_res):
        try:
            self.uncert
        except AttributeError:
            self.uncert = []
        self.uncert.append(u_res)

    def path_by_dir(self):
        led_step        = self.led_step
        ang_z           = self.ang_z
        self.pos_list   = self.start_pos
        for i in np.arange(1,self.no_pos):
            x_pos = self.pos_list[i-1][0] + math.cos(ang_z/180*math.pi)*led_step[i-1]
            y_pos = self.pos_list[i-1][1] + math.sin(ang_z/180*math.pi)*led_step[i-1]
            self.pos_list.append([x_pos,y_pos,0])
        
    def path_by_pos(self, pos_list):       
        self.pos_list   = pos_list
        self.led_step   = []
        self.ang_z      = []
        
        for i in np.arange(1,len(pos_list)):
            x_step = pos_list[i][0]-pos_list[i-1][0]
            y_step = pos_list[i][1]-pos_list[i-1][1]
            hyp = (x_step**2 + y_step**2)**0.5
            
            if y_step == 0 and x_step >= 0:
                ang = 0
            elif x_step == 0 and y_step > 0:
                    ang = math.pi/2
            elif x_step == 0 and y_step < 0:
                    ang = -math.pi/2
            else:
                if x_step > 0:
                    ang = math.atan(y_step/x_step)
                else:
                    if y_step > 0:
                        ang = math.atan(-x_step/y_step)+math.pi/2
                    else:
                        ang = -math.atan(-x_step/-y_step)-math.pi/2                        
            
            self.led_step.append(hyp)
            self.ang_z.append(ang/math.pi*180)
            
    def add_max_distances(self, max_distances):
        self.max_distance = max_distances
        
    def add_sensor(self, sensor):
        self.sensors.append(sensor)