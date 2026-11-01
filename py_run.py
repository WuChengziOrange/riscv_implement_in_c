# %%
import subprocess
from pathlib import Path

prog_path = "./build/emu_runner/emu_runner"
bin_path = Path("./bin")
bin_files = sorted(bin_path.glob("*.bin"))


def run_tests():
    for test_bin in bin_files:
        result = subprocess.run(
            [prog_path, test_bin],
            capture_output=True,  # 捕获 stdout、stderr
            text=True,  # 输出按字符串返回（不是bytes）
            check=False,  # 不自动抛异常，你自己判断返回码
        )
        if result.returncode != 0:
            print(f"Error running {test_bin}:")
            print("return code:", result.returncode)
            print("stdout:\n", result.stdout)
            print("stderr:\n", result.stderr)
            return
        else:
            print(f"Test {test_bin} passed.")
    print("All tests passed successfully!")


run_tests()
