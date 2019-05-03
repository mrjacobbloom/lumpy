# Lumpy

This is Lumpy, a landmass editor and my final project for CSCI 4239 Advanced
Computer Graphics.

![Demo of the app in action](https://raw.githubusercontent.com/mrjacobbloom/lumpy/master/demo.gif)

There are 2 modes: God Mode and 1st-Person Mode. In God Mode, click anywhere on
the sandy plane to create a lump. As the height increases, the texture will
change to grass/plants, then to rock, and then to snow and snow will begin to
fall. Drag to rotate the camera.

Then, switch to 1st-Person Mode by clicking the "First Person" button. Use the
WASD keys or arrow buttons to navigate around the land you designed in God Mode
and drag to rotate the camera.

## Building

This requires Qt. I used Qt 5.9.5.

```shell
qmake && make
```

## Instructions

| Key/Action              | Result                          |
| ----------------------- | ------------------------------- |
| Move mouse              | Move the cursor on the plane    |
| Click/Space/Draw button | Draw a lump                     |
| Drag mouse              | Change the camera angle         |
| WASD/Arrow buttons      | **1st-Person Mode:**  Move the camera <br/> **God Mode:** Move the cursor (relative to camera) |
| Scroll wheel            | Zoom the view                   |
| Cursor Size             | Change the radius of the cursor |

## Technical Details

- Placing the cursor at the mouse position is achieved by rendering the land
  plane to a buffer with a shader where the r and g values at each frag reflect
  the texture coordinates. Then, determining the cursor location is as simple as
  grabbing the buffer's color at the mouse position.
- Normals are calculated based on the height of surrounding pixels on the
  texture when a lump is created, and stored in the GBA components of the
  texture.
- The normals for the water are based on the derivetive of the sinusoidal
  function used to calculate their y value.
- The camera angle is rotated by [a fraction of] the angle between the normal at
  its position and vertical, which means you'll look up slightly as you climb up
  and look down slightly as you climb down.
- First-person velocity calculations and damping are handled in a timer that
  runs 60 times a second because we can't trust vsync.
- Plants are rendered using a Display List (borrowed from
[last year's final project](https://github.com/mrjacobbloom/fruit)). The height
under each plant is determined using an image export of the lump map buffer
generated when a lump is added.

## Credits

This uses code adapted from examples 3, 8, and 12 in my class. Original code
[here](http://www.prinmath.com/csci5229/Sp19/programs/index.html).

The easing function used for
lumps is adapted from [here](https://codepen.io/zadvorsky/pen/qNdrmR).

### Textures

I'm hoping this qualifies as fair use as a school project because I didn't take
care to find royalty-free textures. If I get any cease and desist letters I will
comply.

- [Skybox](http://www.custommapmakers.org/skyboxes.php) by Sorbet
- [Rock](https://texturify.com/stock-photo/mountain-rock057-8916.html)
- [Grass](https://www.textures.com/download/grass0131/38957)
- [Water](https://www.textures.com/preview/waterplain0012/50937)
- [Sand](https://www.textures.com/download/soilbeach0087/32630)
- [Leaf](https://blenderartists.org/t/realistic-grass-attempt/618665/18)