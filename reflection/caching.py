import json
import os
import hashlib

def md5_speedcheck(path, size):
    m = hashlib.md5()
    with open(path, 'rb') as f:
        b = f.read(size)
        while len(b) > 0:
            m.update(b)
            b = f.read(size)
    return m.hexdigest()

def load_cache(build_dir):
    cache_path = os.path.join(build_dir, "reflection_cache.json")
    try:
        if os.path.exists(cache_path):
            with open(cache_path, "r") as f:
                return json.load(f)
    except:
        pass
    return {}

def save_cache(build_dir, cache):
    cache_path = os.path.join(build_dir, "reflection_cache.json")
    with open(cache_path, "w") as f:
        json.dump(cache, f, indent=4, sort_keys=True)