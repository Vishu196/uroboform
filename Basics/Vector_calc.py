# -*- coding: utf-8 -*-
"""
Created on Tue Apr  6 14:40:37 2021

@author: Marin
"""
import math
import numpy as np

# # Punkt
# Ort = [x,y,z] # Ortsvektor

# # Gerade - line
# Richtung = [x_L, y_L, z_L]
# L = Ort + k1*Richtung
class Line:  
    # Attribute
    def __init__(self, vpos, vdir):
        self.vpos = np.array(vpos)
        self.vdir = np.array(vdir)

def create_line (P1,P2):
    line = Line(P1, np.array(P2)-np.array(P1))
    return line

def create_line_2D (P1,P2):
    P1_2D = [P1[2], P1[0]] # [z,x]
    P2_2D = [P2[2], P2[0]]
    line = create_line (P1_2D, P2_2D)
    return line
   
# # Ebene - plane
# E = Ort + k1*Richtung_1 + k2*Richtung_2
class Plane:  
    # Attribute
    def __init__(self, vpos, vdir0, vdir1):
        self.vpos = np.array(vpos)
        self.vdir0 = np.array(vdir0)
        self.vdir1 = np.array(vdir1)

# find point on line with one defined coordinate
def line_coordinates(l_0, index, val):
    s_0 = (val - l_0.vpos[index])/l_0.vdir[index] # Parameter von vektorieller Geradengleichung
    P_0 = l_0.vpos + s_0 *l_0.vdir        
    return P_0

# Schnittpunkt zwischen line und line
def line_intersec(l_0, l_1):
    o_0 = l_0.vpos
    r_0 = l_0.vdir
    o_1 = l_1.vpos
    r_1 = l_1.vdir
    R = np.column_stack((r_0, -1*r_1))
    o = o_1-o_0
    k = np.linalg.tensorsolve(R, o) # Löst A*x = b für x
    S = k[0]*r_0+o_0
    return S

# Schnittpunkt zwischen line und plane
def lp_intersec(l_0, p_1):
    o_0 = l_0.vpos
    r_0 = l_0.vdir
    o_1 = p_1.vpos
    r_1 = p_1.vdir0
    r_2 = p_1.vdir1
    R = np.column_stack((r_0, -1*r_1, -1*r_2))
    o = o_1-o_0
    k = np.linalg.tensorsolve(R, o) # Löst A*x = b für x
    S = k[0]*r_0+o_0
    return S

def plane_intersec(p_0,p_1):
    line_00 = Line(p_0.vpos, p_0.vdir0)
    line_01 = Line(p_0.vpos + p_0.vdir0, p_0.vdir1)
    point0  = lp_intersec(line_00,p_1)
    point1  = lp_intersec(line_01,p_1)
    line_res = create_line(point0, point1)
    return line_res

# Project Point from pattern to sensor by LED
def proj_point (LED, point, plane):
    ray = create_line(LED, point)
    point_proj = lp_intersec(ray, plane)
    return point_proj    

# project line from point
def proj_line (point, line):
    l_pos0 = line.vpos
    l_pos1 = line.vpos + line.vdir
    p_dir0 = l_pos0 - point
    p_dir1 = l_pos1 - point
    plane = Plane(point, p_dir0, p_dir1)
    return plane 

# Rotationsmatrizen
def rot_mat (Phi):
    # Phi   - Lagewinkelvektor
    phi     = Phi [0] # Rotation um x
    theta   = Phi [1] # Rotation um y
    psi     = Phi [2] # Rotation um z
    
    R_x = np.array([[1, 0, 0],
           [0, math.cos(phi), math.sin(phi)],
           [0, -math.sin(phi), math.cos(phi)]])
    
    R_y = np.array([[math.cos(theta), 0, -math.sin(theta)],
           [0, 1, 0],
           [math.sin(theta), 0, math.cos(theta)]])
    
    R_z = np.array([[math.cos(psi), math.sin(psi), 0],
           [-math.sin(psi), math.cos(psi), 0],
           [0, 0, 1]])
    return R_x, R_y, R_z

def rot_SM (rot_0, Phi, pos_M):
    # Rotation aus Maschinenkoordinatensystem in lokales Koordinatensystem
    # rot_0 - Rotationszentrum
    # pos_M - Position, des betrachteten Punkts im Maschinen-KS
    # Phi   - Lagewinkelvektor
    R_x, R_y, R_z = rot_mat(Phi)
    # R_0 = np.matmul(R_y, R_z)
    R_SM = np.matmul(np.matmul(R_x, R_y),R_z)
    pos_S = np.dot(R_SM, (pos_M-rot_0))
    return pos_S

def rot_MS (rot_0, Phi, vec_S):
    # Rotation aus lokalem Koordinatensystem in Maschinenkoordinatensystem
    # rot_0 - Rotationszentrum im Maschinen-KS
    # pos_0 - Vektor im Sensor-KS
    # Phi   - Lagewinkelvektor zwischen Sensor-KS und Maschinen-KS
    R_x, R_y, R_z = rot_mat(Phi)
    R_0 = np.matmul(np.matmul(R_x, R_y),R_z)
    R_MS = R_0.T
    
    vec_M = np.dot(R_MS, vec_S) + rot_0
    return vec_M

def rot_plane_MS(Phi, plane):
    # rot_0 = plane.vpos
    rot_0 = np.array([0,0,0])
    vdir0_M = rot_MS (rot_0, Phi, plane.vdir0)
    vdir1_M = rot_MS (rot_0, Phi, plane.vdir1)
    plane_M = Plane(plane.vpos, vdir0_M, vdir1_M)
    return plane_M

def rot_plane_SM(Phi, plane):
    # rot_0 = plane.vpos
    rot_0 = np.array([0,0,0])
    vdir0_S = rot_SM (rot_0, Phi, plane.vdir0)
    vdir1_S = rot_SM (rot_0, Phi, plane.vdir1)
    plane_S = Plane(plane.vpos, vdir0_S, vdir1_S)
    return plane_S    