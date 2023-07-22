import matplotlib.pyplot as plt

plt.rcParams["text.usetex"] = True
plt.rcParams["font.size"] = "14"
plt.rcParams['text.latex.preamble'] = r"\usepackage{upgreek}"
import numpy as np
import pandas as pd
# import scipy

import pickle
from scipy.stats import linregress
import time

from Basics import exp_params
import Evaluation

# %% import

plot = False
mode = 'gauss'
regular = True

# import grid design 
[grid_height, grid_width], look_up = Evaluation.Bildfun.grid_params()

# import images from folder
folder =  'D:\\Vaishnavi\\Master Thesis\\uroboformV\\images'   #'G:\\mte\\PMMF\\CMM-07-05'
# breakpoint()
im_range = [6460, 6630]
images = exp_params.read_images(folder)#, num=im_range)

# mask & cam parameter
h1 = 800
h2 = 20
px_size = 3.45
stripe_width = 100

# Referenz-Positionen
# pos = import_manager.calibration_ref('CMM-07-10')

# %% image processing
xi_i = []
zeta_i = []
loc = []

count = 0
duration = []
time_list = []

grid_list = []
for image0 in images[:]:
    # breakpoint()
    # split images im grinds 
    start = time.time()
    # image = scipy.ndimage.rotate(image0, 0.6, mode = "nearest")
    # breakpoint()
    xi_ret, zeta_ret, index, ori, k, grids, times = \
        Evaluation.main_image_grid.grid_pos(image0, mode=mode)
    stop = time.time()
    times.insert(0, start)
    times.append(stop)
    # print(stop-start)
    if count % 20 == 0:
        print(count)
    #     plt.imshow(image0,'gray')
    #     plt.show()# print(count)
    im_size = np.size(image0)
    duration.append(stop - start)
    xi_i.append(xi_ret)
    zeta_i.append(zeta_ret)
    loc.append([index, ori, k])
    grid_list.append(grids)

    time_list.append(times)
    # mp_res.append(im_0)

    count += 1

time_arr = np.array(time_list)#[20:150])
print("time array: "+time_arr)
time_diff = time_arr[:, 1:] - time_arr[:, :-1]
print(np.mean(time_diff, 0))

# %% save results
results = np.zeros((len(loc), 5), dtype=object)
results[:, 0] = xi_i
results[:, 1] = zeta_i

index_list = [loc[i][0] for i in np.arange(len(loc))]
ori_list = [loc[i][1] for i in np.arange(len(loc))]
k_list = [loc[i][2] for i in np.arange(len(loc))]

results[:, 2] = np.array(index_list)
results[:, 3] = np.array(ori_list)
results[:, 4] = np.array(k_list)

res_table = pd.DataFrame(results)
res_table.columns = 'xi_i', 'zeta_i', 'index', 'orientation', 'k'

if regular:
    res_name = folder + '\\results_' + f'{im_range[0]:05d}' + '-' + f'{im_range[1]:05d}' + '.csv'
else:
    res_name = folder + '\\phase_results_' + f'{im_range[0]:05d}' + '-' + f'{im_range[1]:05d}' + '.csv'

res_table.to_csv(res_name, index=False)

# %% evaluate CNR
noise_cv = []
for i in np.arange(0, len(images), 10):
    im_ar = np.array(images[i:i + 10])
    im0 = np.mean(im_ar, 0)
    int_range = np.percentile(im0, 95) - np.percentile(im0, 5)
    noise_cv.append(int_range / np.mean(np.std(im_ar, 0)))

cnr_table = pd.DataFrame(np.array(noise_cv))
cnr_name = folder + '\\cnr_' + f'{im_range[0]:05d}' + '-' + f'{im_range[1]:05d}' + '.csv'

cnr_table.to_csv(cnr_name, index=False)
