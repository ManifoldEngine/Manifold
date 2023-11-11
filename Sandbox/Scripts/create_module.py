import argparse
import locations as locations
from "../../Engine/Scripts/module_creator" import create_module

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a new module at project source location")
    parser.add_argument("module_name", type=str, help="Name of the new module")
    
    args = parser.parse_args()
    create_module(args, locations)