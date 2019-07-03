import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
from scipy.linalg import norm

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
origin = np.array([0, 0, 0])

#radius
R = 1
#vector in direction of axis
v = np.array([0, 0, 1])
#find magnitude of vector
mag = norm(v)
#unit vector in direction of axis
v = v / mag
#make some vector not in the same direction as v
not_v = np.array([1, 0, 0])
#make vector perpendicular to v
n1 = np.cross(v, not_v)
#normalize n1
n1 /= norm(n1)
#make unit vector perpendicular to v and n1
n2 = np.cross(v, n1)
#surface ranges over t from 0 to length of axis and 0 to 2*pi
t = np.linspace(0, mag, 100)
theta = np.linspace(0, 1 * np.pi, 100)
#use meshgrid to make 2d arrays
t, theta = np.meshgrid(t, theta)
#generate coordinates for surface
X, Y, Z = [v[i] * t + R * np.sin(theta) * n1[i] + R * np.cos(theta) * n2[i] for i in [0, 1, 2]]
ax.plot_surface(X, Y, Z, cmap=cm.Blues, linewidth=0)#, antialiased=False)

mu_xdata = np.loadtxt("mu_x.txt")
mu_ydata = np.loadtxt("mu_y.txt")
mu_zdata = np.loadtxt("mu_z.txt")
ax.scatter3D(mu_xdata, mu_ydata, mu_zdata, c=mu_zdata, cmap=cm.Reds);

ph_xdata = np.loadtxt("ph_x.txt")
ph_ydata = np.loadtxt("ph_y.txt")
ph_zdata = np.loadtxt("ph_z.txt")
ax.scatter3D(ph_xdata, ph_ydata, ph_zdata, c=ph_zdata, cmap=cm.Greens);

#space axis
ax.set_xlim(-1.1, 1.1)
ax.set_ylim(-1.1, 1.1)
ax.set_zlim(0, 1.1)
plt.show()
