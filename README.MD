# Unreal Engine FLECS Integration

This is an implementation of [flecs](https://github.com/SanderMertens/flecs) which'll work more natively with Unreal Engine itself.

## Setup

Clone this project to your **"YourProject/Plugins/"** as a submodule. Then try to compile and enable through your project of your choice.

## Why this project?

This project will be a hub for future Unreal Engine users to get into ECS. It'll contain implementations and example so that users can get into ECS easily without much headaches. I saw few implementations like [this](https://github.com/PreyK/Unreal-Minimum-Viable-Flecs) and [this](https://github.com/jtferson/UnrealFlecsQuickstart) which is a nice example but doesn't solve multithreading and pipeline problem. Also FSTicker doesn't exactly called from game world rather somewhat directly from Main Loop itself. Which is a bit sucky to work with and multithreading it can cause crashes due to interfering with ticks where it shouldn't. So this project hopefully be a solution to this problem.

## Why integrate FLECS into Unreal Engine? Why not just use Unreal Engine's Mass?

FLECS is a pretty fast and lightweight ECS system. Also its quite simple to setup on any project. Just including the flecs.c and flecs.h can easily make it work on any system without much headache. Personally speaking there are several reasons why I am picking flecs over Mass. Take these opinion with a grin of salt.

For Mass:
- Firstly Mass to me looks quite messy with inheritance system and making mess of inheritance system is very much against the principal of ECS
- Mass is still at its infancy as of now.
- Mass got confusing terms Fragments, Processors, SharedFragments, etc. atleast for me to understand.
- Got some complicated process to get systems working.

For FLECS:
- It is much more done than Mass for ECS Solution.
- You can communicate with Unreal's Actor and FLECS quite easily thus makes your converting your codes to FLECS much easily.
- Comes with nice examples on [flecs](https://github.com/SanderMertens/flecs/tree/master/examples) repo to understand how it works.

## Challenges to FLECS integration in Unreal Engine.

FLECS regardless comes with few challenges. We already somewhat discussed the problem and now we have to do few things to solve it.
- Adding proper place to Tick. Atleast more native with Actor or ticking from WorldSubsystem with more condition to prevent ticks on viewport. Also making or pipelining ticks to Tickgroup so that we can set our systems properly and where to tick. Achieving proper sync point for multithreading is one of the biggest challenge.
- Adding multithreading support. Thanks to [Megafunk](https://github.com/Megafunk). I somewhat got close to the problem but still may need other stuffs to do...
- One of the key features of ECS is to increase reusability. We need to find a proper solution so that a user can declare components and systems much easily.
- Making some brief examples to communicate between ECS World and Unreal Engine's Actor world for showing basic conversion process.

## Future works
- **Adding Physics Support:** Physics support can be difficult and relies heavily on what Unreal Engine will do on future with its Engine. As of now in 10 Feb 2024, Chaos Physics is still in primitive stage from switch from Nvidia's PhysX Engine. Physics is single threaded and got inferior performance. Low level access to Physics not easy as it is heavily integrated with Actor system of Unreal Engine. Alternative Solution could be [Bullet Physics](https://github.com/bulletphysics/bullet3) since its much more completed, deterministic and performant.
- **Adding Navigation support:** Navigation in Navmesh looks much easy to integrate. We can just use its pathfinding system directly to our ECS World via singletons.
- **Trying to multithread existing components of Unreal Engine:** We can try to multithread existing components. Most notably, Character Movement Component of Unreal Engine. Which has notorious reputation for being unoptimized and thanks to being singlethreaded you can't put more units on it. We can start with making it safe to be multithreaded or simply re-write it into FLECS components and systems.
- **Converting popular community driven plugins to FLECS:** We can try converting community driven plugins/assets like ALS(Advanced Locomotion System) into FLECS.
