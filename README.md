# simple_cfd

## TODO
- [ ] Bring in the multi-resolution graph as a submodule, instead of just as files
- [ ] ~Model "Euler Equations" https://en.wikipedia.org/wiki/Euler_equations_(fluid_dynamics)~ (decided to just go right for Navier-Stokes, not much harder and gives better results)
- [ ] Model "Navier-Stokes" https://en.wikipedia.org/wiki/Navier%E2%80%93Stokes_equations and solve for velocity
- [ ] Extract pressure from Navier-Stokes velocity field

## Notes
- The Navier-Stokes is just a generalized version of the Euler equations

## Ideas

### Grid Splitting
- When the pressure change over a timestep exceeds a threshold for a point, split that point (and vice versa, merge points with little change)
