# Test-That-Robot
Test environment for motion planning methods of holonomic mobile robots

## Stage 1: Concept and Goals

The goal of the first stage is to create a simple simulation environment for mobile robots, inspired by solutions like Gazebo and Webots, but much easier to use. The environment will allow:

- Quick and intuitive configuration of a scene with obstacles and a robot.
- Visualization of robot movement and obstacles using an OpenGL widget.
- Adding a widget for writing and running control algorithms directly in the environment, with immediate testing.

### Planned actions for Stage 1

1. **Concept description**
   - Define functional and technical requirements.
   - Compare with existing solutions (Gazebo, Webots).

2. **Implementation of basic functionality**
   - Add an OpenGL widget for visualization of obstacles and robot movement.
   - Add a widget for programming and running control algorithms.

3. **Testing and verification**
   - Check if the environment meets usability assumptions.
   - Verify the possibility of programming and testing control algorithms.

### Progress

- Main project assumptions defined.
- Architecture design started.

### Next steps ()

- Implement OpenGL visualization widget.
- Implement control algorithm programming widget.
- Functional and usability testing.

### set_the_box

This stage focuses on starting work with OpenGL integration.  
The main goal is to add the ability to create and visualize a simple box (obstacle) in the simulation environment.  
This will be the foundation for further development of the visualization module and for representing obstacles and robot movement.

**Planned actions:**
- [x] Create basic UI for app 
- [x] Integrate an OpenGL widget into the environment.
- [x] Implement basic functionality to add and display a box.
- [x] Create basic UI for adding and manipulating the box.
- [x] Test rendering and interaction with the box object.

### 

Robot introduction milestone: add the first robot entity to the OpenGL scene (visual mesh + basic kinematic/physics placeholder).

**its_alive Objectives**
- Visual presence of a robot
- Updatable pose (x, y, theta)
- Interface for control algorithms to set velocities

**Planned Actions**
- [x] Create robot mesh (start with simple triangle; )
- [x] Define robot pose structure (position, orientation; world frame documented)
- [ ] Integrate mesh into render loop (reuse existing shaders / buffers)
- [ ] Implement basic holonomic kinematics (apply vx, vy, omega per tick)
- [ ] Add simple physics placeholder (velocity limits, optional damping)
- [ ] Expose control hook (temporary UI fields or stub algorithm callback)
- [ ] Generate AABB from mesh for future collision checks
- [ ] Update README with coordinate frame + units

**Acceptance Criteria**
- Robot appears in scene at default pose
- Pose updates smoothly when velocities change
- Frame resize / redraw works without artifacts
- Code separated (e.g., robot/Robot.h|cpp or robot.py)
- Single control entry point (setVelocity / update(dt))
- No runtime GL errors in debug/log
