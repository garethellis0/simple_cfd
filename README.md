# simple_cfd


**TODO: Project Decription here**
- uses `boost::units` to help ensure correctness
- uses a custom multi-resolution graph implementation to ensure there is higher resolution in areas of greater change

## Cloning 
- simple run `git clone --recursive git@github.com:garethellis0/simple_cfd.git` to get this project with its dependencies
 
## TODO
- [x] Bring in the multi-resolution graph as a submodule, instead of just as files
- [x] ~Model "Euler Equations" https://en.wikipedia.org/wiki/Euler_equations_(fluid_dynamics)~ (decided to just go right for Navier-Stokes, not much harder and gives better results)
- [x] Model "Navier-Stokes" https://en.wikipedia.org/wiki/Navier%E2%80%93Stokes_equations and solve for velocity (somewhat done, modeled Navier-Stokes with slight compressability via a pressure approx. from velocity)
- [x] Extract pressure from Navier-Stokes velocity field
- [x] Clean up the graph rendering code and change the render model to allow for update steps longer then the render cycle time
- [ ] Build an (programmatic) interface for adding obstacles
- [ ] Come up with a better visualisation scheme
- [ ] Change the update model to use interpolation and generate a totally new graph each time, instead of just updating the existing one (prep for dynamic mesh generation)
- [ ] Implement basic mesh generation (higher resolution in areas that need it)
- [ ] Build a basic GUI for setting up the configuration parameters (min/max mesh resolution, update time step, etc.)
- [ ] Add basic GUI for adding obstacles (maybe upload PNG's drawn in paint?)

## Notes
- The Navier-Stokes is just a generalized version of the Euler equations

## Ideas

### Grid Splitting
- When the pressure change over a timestep exceeds a threshold for a point, split that point (and vice versa, merge points with little change)

## Progress
June 2nd, 2018:
![screenshot from 2018-07-02 06-10-57](https://user-images.githubusercontent.com/9075711/42166058-b97058f2-7dbe-11e8-9962-97a9cb1b7200.png)
August 30th, 2018 (took a bit of a break...):
![demo](https://user-images.githubusercontent.com/9075711/44887938-cdeb9480-ac8b-11e8-8aae-cdc133495163.gif)
