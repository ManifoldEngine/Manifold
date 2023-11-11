import os


def fill_template(template_path, template_args):
    with open(template_path, 'r') as file:
        template_content = file.read()
    for key, value in template_args.items():
        template_content = template_content.replace(f"#{key}#", value)
    return template_content

def create_directory(path):
    print(f"Creating directory at {path}")
    try:
        os.mkdir(path)
    except FileExistsError:
        print(f"\'{path}\' already exists.")
    except Exception:
        raise

def create_file(path, file_name, file_content):
    file_path = os.path.join(path, file_name)
    print(f"Creating file at {file_path}")
    with open(file_path, 'w') as file:
        file.write(file_content)
        
def append_line(path, line):
    with open(path, 'r') as file:
        file_content = file.read()

    if line not in file_content:
        with open(path, 'a') as file:
            if file.tell() > 0:
                file.write("\n")
            file.write(line)

def create_module(args, config):
    base_path = os.path.join(config.project_dir, config.modules_dir)
    module_path = os.path.join(base_path, args.module_name)
    sources_path = os.path.join(module_path, args.module_name)
    tests_path = os.path.join(module_path, "Tests")

    premake5lua_template = os.path.join(config.templates_dir, "premake5lua.template")
    module_header_template = os.path.join(config.templates_dir, "ModuleHeader.h.template")
    module_source_template = os.path.join(config.templates_dir, "ModuleHeader.cpp.template")
    module_tests_main_template = os.path.join(config.templates_dir, "ModuleTests.cpp.template")

    try:
        create_directory(module_path)
        create_directory(sources_path)
        create_directory(tests_path)
        
        template_args = {
            "ModuleName": args.module_name,
        }

        create_file(module_path, "premake5.lua", fill_template(premake5lua_template, template_args))
        create_file(sources_path, f"{args.module_name}.h", fill_template(module_header_template, template_args))
        create_file(sources_path, f"{args.module_name}.cpp", fill_template(module_source_template, template_args))
        create_file(tests_path, f"{args.module_name}Tests.cpp", fill_template(module_tests_main_template, template_args))

        append_line(f"{base_path}/premake5.lua", f"include \"{args.module_name}\"")

    except Exception:
        raise