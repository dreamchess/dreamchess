#!BPY

#  DreamChess
#  Copyright (C) 2005  The DreamChess project
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

"""
Name: 'DreamChess Model (.dcm)...'
Blender: 232
Group: 'Export'
Tooltip: 'Export selected mesh to DreamChess Model format (.dcm)'
"""

import Blender, mod_meshtools
import sys

def write_vtx(f, vtx):
  f.write("%f %f %f " % (vtx.co[0], vtx.co[1], vtx.co[2]))
  f.write("%f %f %f\n" % (vtx.no[0], vtx.no[1], vtx.no[2]))

def write_face(f, face):
  if(len(face.v) > 2):
    f.write("%i %i %i" % (face.v[0].index, face.v[1].index, face.v[2].index))
    for v in range (3):
      f.write(" %f %f" % (face.uv[v][0], face.uv[v][1]))
    f.write("\n")
  if(len(face.v) > 3):
    f.write("%i %i %i" % (face.v[2].index, face.v[3].index, face.v[0].index))
    for v in [2, 3, 0]:
      f.write(" %f %f" % (face.uv[v][0], face.uv[v][1]))
    f.write("\n")

def count_triangles(mesh):
  triangles = 0
  for face in mesh.faces:
    if len(face.v) > 3:
      triangles += 2
    else:
      triangles += 1
  return triangles

def write_mesh(f, mesh):
  f.write("%i\n" % len(mesh.verts))
  for vtx in mesh.verts:
    write_vtx(f, vtx)
  f.write("%i\n" % count_triangles(mesh))
  for face in mesh.faces:
    write_face(f, face)

def write(filename):
  objects = Blender.Object.GetSelected()
  meshname = objects[0].data.name
  mesh = Blender.NMesh.GetRaw(meshname)

  file = open(filename, "w")
  file.write("DCM 0000\n")
  write_mesh(file, mesh)
  file.close()

  message = "Successfully exported " + Blender.sys.basename(filename)
  mod_meshtools.print_boxed(message)

def fs_callback(filename):
  if filename.find('.dcm', -4) <= 0: filename += '.dcm'
  write(filename)

Blender.Window.FileSelector(fs_callback, "Export DCM")
