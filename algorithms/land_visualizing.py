import folium
import webbrowser
from folium import plugins
from selenium import webdriver
import os
import time
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import test

elevation_height = test.get_elevation_height()
water_height = np.zeros([elevation_height.shape[0], elevation_height.shape[1]])
EV_cells = np.zeros([elevation_height.shape[0], elevation_height.shape[1]])


west_longitude = -121.7519
east_longitude = -121.7456
north_latitude = 36.9052
south_latitude = 36.9022

# Calculate the coordinates of the bounding box
bounding_box = [(south_latitude, west_longitude), (north_latitude, east_longitude)]

# Calculate the width and height of the bounding box
width = abs(east_longitude - west_longitude)
height = abs(north_latitude - south_latitude)

# Calculate the average latitude for a more accurate fit
average_latitude = (north_latitude + south_latitude) / 2

# Calculate the zoom level based on the bounding box dimensions
zoom_level = 15 - max(width, height) * 2

# Create a map and fit it to the bounding box
m = folium.Map(location=[average_latitude, (west_longitude + east_longitude) / 2], zoom_start=zoom_level, zoomControl=False)
m.fit_bounds(bounding_box)

# Save the map as an HTML file
mapFname = 'output.html'
m.save(mapFname)

mapUr1 = 'file://{0}/{1}'.format(os.getcwd(), mapFname)
driver = webdriver.Chrome()
driver.get(mapUr1)
time.sleep(5)
driver.save_screenshot('output.png')
driver.quit()

# Open the image
image = mpimg.imread('output.png')

# Create a Matplotlib figure and axis
fig, ax = plt.subplots()

# Display the image
ax.imshow(image)

# Add a grid that fits the size of the land
grid = np.zeros_like(image)
grid[::20, :] = 1  # Adjust the grid spacing as needed
grid[:, ::20] = 1  # Adjust the grid spacing as needed

# Display the grid on top of the image
# Invert the grid array
grid = np.flipud(grid)

# Display the grid on top of the image
ax.imshow(grid, cmap='gray', alpha=0.3, extent=[0, image.shape[1], image.shape[0], 0])

# Hide the axes for a cleaner look
ax.axis('off')

# Show the plot and keep it open
plt.show(block=True)