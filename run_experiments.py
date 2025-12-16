import toml
import subprocess
import os

# --- Configuration ---
CONFIG_FILE = 'config.toml'
CPP_EXECUTABLE = './build/run_rtree' # Path to your compiled program
# ---------------------

def main():
    print(f"Loading configuration from {CONFIG_FILE}...")
    try:
        config = toml.load(CONFIG_FILE)
    except Exception as e:
        print(f"Error: Could not parse {CONFIG_FILE}. {e}")
        return

    # Load global settings
    globals = {
        'N': config.get('num_insertions', 100000),
        'variant': config.get('tree_variant', 'LINEAR'),
        'data_type': config.get('data_type', 'RANDOM'),
        'page_size': config.get('page_size_bytes', 4096)
    }
    
    print("Global settings loaded.")

    # Loop through each section in the config file
    for experiment_name, settings in config.items():
        if not isinstance(settings, dict) or not settings.get('run', False):
            continue 

        print(f"\n--- Starting Experiment: [{experiment_name}] ---")

        if 'on_disk' in experiment_name:
            run_type = 'disk'
        elif 'in_memory' in experiment_name:
            run_type = 'mem'
        else:
            print(f"Skipping unknown experiment type: {experiment_name}")
            continue

        # Get specific settings
        M = settings.get('M_capacity', 16)
        fill = settings.get('fill_factor', 0.5)
        N = globals['N']
        variant = globals['variant']
        data_type = globals['data_type']
        page_size = globals['page_size']
        
        # --- NEW BUFFER LOGIC ---
        buffer_type = "NONE"
        buffer_pages = 0
        
        if run_type == 'disk':
            buffer_type = settings.get('buffer_type', 'NONE').upper()
            buffer_mb = settings.get('buffer_size_mb', 0)
            if buffer_mb > 0 and page_size > 0:
                buffer_pages = (buffer_mb * 1024 * 1024) // page_size
        
        # Auto-generate output filename
        output_file = f"{run_type}_M{M}_fill{int(fill*100)}_N{N}_{data_type.lower()}"
        if run_type == 'disk':
            output_file += f"_buf{buffer_type}_{buffer_mb}MB"
        output_file += ".csv"

        # Build the command as a list of strings
        command = [
            CPP_EXECUTABLE,
            run_type,
            str(M),
            str(fill),
            str(N),
            buffer_type,    # <-- NEW ARGUMENT
            str(buffer_pages),  # <-- NEW ARGUMENT
            variant,
            data_type,
            str(page_size),
            output_file
        ]
        
        print(f"Executing: {' '.join(command)}")

        try:
            subprocess.run(command, check=True)
            print(f"--- Finished Experiment: [{experiment_name}] ---")
        except subprocess.CalledProcessError as e:
            print(f"!!! ERROR running [{experiment_name}]: {e}")
        except FileNotFoundError:
            print(f"!!! ERROR: Executable not found at {CPP_EXECUTABLE}")
            print("Did you compile the C++ code? (e.g., cd build && make)")
            break

if __name__ == "__main__":
    main()
