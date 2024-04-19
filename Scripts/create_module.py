import argparse
import locations as locations
import importlib

def create_module(args, config):
    try:
        module_creator = importlib.import_module(f"modules.module_creator")        
        module_creator.create_module(args, config)
    except ImportError:
        print(f"Module 'module_creator' not found.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a new module at project source location")
    parser.add_argument("module_name", type=str, help="Name of the new module")

    args = parser.parse_args()
    create_module(args, locations)