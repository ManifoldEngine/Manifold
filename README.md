# ECSEngine
A simple ECS engine

# TODO 
- Either find a library that's compatible with shared lib or refactor our std containers
- refactor the ComponentPools to use sparse arrays
- Figure out a better solution than the Enity::IsAlive flag for entity recycling. Right now entity ids are not unique and that's messed up.