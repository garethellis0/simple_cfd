# simple_cfd


**TODO: Project Decription here**
- uses `boost::units` to help ensure correctness
- uses a custom multi-resolution graph implementation to ensure there is higher resolution in areas of greater change

## Cloning 
- simple run `git clone --recursive git@github.com:garethellis0/simple_cfd.git` to get this project with its dependencies

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

## Progress
June 2nd, 2018:
![screenshot from 2018-07-02 06-10-57](https://user-images.githubusercontent.com/9075711/42166058-b97058f2-7dbe-11e8-9962-97a9cb1b7200.png)
