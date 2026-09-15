# Breaking changes

## Precision fix (@TODO add date / commit)

This change increased depth precision, and by extension UV and position too by a bit.
However some existing bugs had to be fixed that caused a mismatch of units:

### Projection Near/Far

`t3d_viewport_set_projection` and `t3d_viewport_set_perspective` where wrong internally.
It resulted in a `far` value that was twice as far as it was supposed to be, at least for clipping.
Please re-adjust to the true value you need, otherwise geometry will clip too early now.

### Fog Range

`t3d_fog_set_range` was internally not correctly mapped to the world-space units.
This resulted in its near/far to refer to around double the amount.
Please re-adjust to the true value you need, otherwise fog will be too strong now.

`t3d_fog_set_range` now also requires a viewport to be attached.
Please do so before a call, and re-set fog for every viewport (e.g. in split-screens).

### Near-Clipping

Before, geometry was clipped against the near-plane,
now it does so against the cameras eye instead.
The region between near and the eye will be drawn with a clamped minimum depth value.
Be aware geometry can now be a lot closer to the camera, and ideally make sure the camera doesn't get too close (which was a good idea before the changes too).
If you relied on some kind of effect that artificially moved the near-plane further, this is no longer supported.