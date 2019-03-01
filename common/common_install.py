#
#   =======================================================================
#
# Copyright (C) 2018, Hisilicon Technologies Co., Ltd. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   1 Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#   2 Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#   3 Neither the names of the copyright holders nor the names of the
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#   =======================================================================
#
'''common installation'''
import os
import platform
import signal
import subprocess
import time
import getpass
import json
import pexpect
from collections import OrderedDict
import re
import datetime

#ssh expect prompt
PROMPT = ['# ', '>>> ', '> ', '\$ ']

CURRENT_PATH = os.path.dirname(
    os.path.realpath(__file__))

#installed so files
DEVICE_INSTALLED_SO_FILE = [{"makefile_path": os.path.join(CURRENT_PATH, "utils/ascend_ezdvpp"),
                       "engine_setting": "-lascend_ezdvpp \\",
                       "so_file" : os.path.join(CURRENT_PATH, "utils/ascend_ezdvpp/out/libascend_ezdvpp.so")},
                      {"makefile_path": os.path.join(CURRENT_PATH, "osd"),
                       "engine_setting": "-lascenddk_osd \\",
                       "so_file" : os.path.join(CURRENT_PATH, "osd/out/libascenddk_osd.so")}]

HOST_INSTALLED_SO_FILE = [{"makefile_path": os.path.join(CURRENT_PATH, "presenter/agent"),
                       "engine_setting": "-lpresenteragent \\",
                       "so_file" : os.path.join(CURRENT_PATH, "presenter/agent/out/libpresenteragent.so")}]

MODE_ASIC = "ASIC"
MODE_ATLAS_DK = "Atlas DK"

def execute(cmd, timeout=3600, cwd=None):
    '''execute os command'''
    print(cmd)
    is_linux = platform.system() == 'Linux'

    if not cwd:
        cwd = os.getcwd()
    process = subprocess.Popen(cmd, cwd=cwd, bufsize=32768, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, shell=True,
                               preexec_fn=os.setsid if is_linux else None)

    t_beginning = time.time()

    # cycle times
    time_gap = 0.01

    str_std_output = ""
    while True:

        str_out = str(process.stdout.read().decode())
        str_std_output = str_std_output + str_out

        if process.poll() is not None:
            break
        seconds_passed = time.time() - t_beginning

        if timeout and seconds_passed > timeout:

            if is_linux:
                os.kill(process.pid, signal.SIGTERM)
            else:
                process.terminate()
            return False, process.stdout.readlines()
        time.sleep(time_gap)
    str_std_output = str_std_output.strip()
    # print(str_std_output)
    std_output_lines_last = []
    std_output_lines = str_std_output.split("\n")
    for i in std_output_lines:
        std_output_lines_last.append(i)

    if process.returncode != 0 or "Traceback" in str_std_output:
        return False, std_output_lines_last

    return True, std_output_lines_last

def read_ddk_info(ddk_info_path):
    try:
        ddk_info_file = open(
            ddk_info_path, 'r', encoding='utf-8')
        ddk_info_dict = json.load(
            ddk_info_file, object_pairs_hook=OrderedDict)
        
        if "TARGET" in ddk_info_dict:
            mode = ddk_info_dict.get("TARGET")
        else:
            mode = MODE_ATLAS_DK
            
        
        return True, mode
    except OSError as reason:
        print("read ddk_info file failed")
        exit(-1)
    finally:
        if ddk_info_file in locals():
            ddk_info_file.close()

    return False, ""

def scp_file_to_remote(user, ip, port, password, local_file, remote_file):
    '''do scp file to remote node'''
    cmd = "scp -P {port} {local_file} {user}@{ip}:{remote_file} ".format(
        ip=ip, port=port, user=user, local_file=local_file, remote_file=remote_file)
    print(cmd)
    try:
        process = pexpect.spawn(cmd)

        ret = process.expect(
            ["[Pp]assword", "Are you sure you want to continue connecting"])
        if ret == 1:
            process.sendline("yes")
            print(process.before)
            ret = process.expect("[Pp]assword")
        if ret != 0:
            return False

        process.sendline(password)
        process.read()
        print(process.before)
        process.expect(pexpect.EOF)
    except:
        return False
    finally:
        process.close(force=True)
    return True


def ssh_to_remote(user, ip, port, password, cmd_list):
    '''ssh to remote and execute command'''
    cmd = "ssh -p {port} {user}@{ip} ".format(ip=ip, port=port, user=user)
    print(cmd)
    try:
        process = pexpect.spawn(cmd)
        ret = process.expect(
            ["[Pp]assword", "Are you sure you want to continue connecting"])
        if ret == 1:
            process.sendline("yes")
            ret = process.expect("[Pp]assword")
        if ret != 0:
            return False

        process.sendline(password)
        process.expect(PROMPT)

        for cmd in cmd_list:
            if cmd.get("type") == "expect":
                process.expect(cmd.get("value"))
            else:
                if cmd.get("secure") is False:
                    print(cmd.get("value"))
                process.sendline(cmd.get("value"))
    except:
        return False
    finally:
        process.close(force=True)
    return True


def add_engine_setting(mode, host_engine_settings, device_engine_settings):
    '''add common so configuration to ddk engine default setting file'''
    ddk_engine_config_path = os.path.join(
        os.getenv("DDK_HOME"), "conf/settings_engine.conf")
    try:
        ddk_engine_config_file = open(
            ddk_engine_config_path, 'r', encoding='utf-8')
        ddk_engine_config_info = json.load(
            ddk_engine_config_file, object_pairs_hook=OrderedDict)
        
        if mode == MODE_ASIC:
            engine_info = ddk_engine_config_info.get(
            "configuration").get("FPGA")
        else:
            engine_info = ddk_engine_config_info.get(
            "configuration").get("OI")
        
        host_engine_link_obj = engine_info.get("Host").get("linkflags").get("linkobj")
        device_engine_link_obj = engine_info.get("Device").get("linkflags").get("linkobj")

        for each_new_setting in host_engine_settings:
            if each_new_setting not in host_engine_link_obj:
                host_engine_link_obj.insert(-1, each_new_setting)
        for each_new_setting in device_engine_settings:
            if each_new_setting not in device_engine_link_obj:
                device_engine_link_obj.insert(-1, each_new_setting)
        ddk_engine_config_new_file = open(
            ddk_engine_config_path, 'w', encoding='utf-8')
        json.dump(ddk_engine_config_info, ddk_engine_config_new_file, indent=2)
    except OSError as reason:
        print("read egine setting file failed")
        exit(-1)
    finally:
        if ddk_engine_config_file in locals():
            ddk_engine_config_file.close()
        if ddk_engine_config_new_file in locals():
            ddk_engine_config_new_file.close()


def main():
    '''main function: install common so files'''
    ddk_engine_config_path = os.path.join(
        os.getenv("DDK_HOME"), "conf/settings_engine.conf")
    if not os.path.exists(ddk_engine_config_path):
        print("Can not find setings_engine.conf, please check DDK installation.")
        exit(-1)
    
    ddk_info_path = os.path.join(
        os.getenv("DDK_HOME"), "ddk_info")
    if not os.path.exists(ddk_info_path):
        print("Can not find ddk_info, please check DDK installation.")
        exit(-1)
    
    ret, mode = read_ddk_info(ddk_info_path)
    if not ret:
        exit(-1)

    while(True):
        developer_ip = input("Please input %s Device IP:" % mode)

        if re.match(r"^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$", developer_ip):
            break
        else:
            print("Input IP: %s is invalid!" % developer_ip)

    developer_ssh_user = input(
        "Please input %s Device SSH user(default: HwHiAiUser):" % mode)
    if developer_ssh_user == "":
        developer_ssh_user = "HwHiAiUser"

    developer_ssh_pwd = getpass.getpass(
        "Please input %s Device SSH user password:" % mode)

    developer_ssh_port = input("Please input %s Device SSH port(default: 22):" % mode)
    if developer_ssh_port == "":
        developer_ssh_port = "22"

    if developer_ssh_user != "root":
        developer_root_pwd = getpass.getpass(
            "Please input %s Device root user password:" % mode)
    
    
    print("Common installation is beggining.")

    host_engine_settings = []
    device_engine_settings = []
    now_time = datetime.datetime.now().strftime('scp_lib_%Y%m%d%H%M%S')

    mkdir_expect = PROMPT
    mkdir_expect.append("File exists")
    ret = ssh_to_remote(developer_ssh_user, developer_ip, developer_ssh_port,
                  developer_ssh_pwd, [{"type": "cmd",
                                     "value": "mkdir ./{scp_lib}".format(scp_lib=now_time),
                                     "secure": False},
                                    {"type": "expect",
                                     "value": mkdir_expect}])
    if not ret:
        print("Mkdir dir in %s failed, please check your env and input." % developer_ip)
        exit(-1)

    for each_so_file_dict in HOST_INSTALLED_SO_FILE:
        makefile_path = each_so_file_dict.get("makefile_path")
        host_engine_settings.append(each_so_file_dict.get("engine_setting"))
        so_file = each_so_file_dict.get("so_file")

        execute("make clean -C {path}".format(path=makefile_path))

        execute("make install mode={mode} -C {path}".format(mode=mode.replace(" ", ""), path=makefile_path))
        if mode == MODE_ATLAS_DK:
            ret = scp_file_to_remote(developer_ssh_user, developer_ip, developer_ssh_port,
                               developer_ssh_pwd, so_file, "./{scp_lib}".format(scp_lib=now_time))

    for each_so_file_dict in DEVICE_INSTALLED_SO_FILE:
        makefile_path = each_so_file_dict.get("makefile_path")
        device_engine_settings.append(each_so_file_dict.get("engine_setting"))
        so_file = each_so_file_dict.get("so_file")

        execute("make clean -C {path}".format(path=makefile_path))

        execute("make install mode={mode} -C {path}".format(mode=mode.replace(" ", ""), path=makefile_path))

        ret = scp_file_to_remote(developer_ssh_user, developer_ip, developer_ssh_port,
                           developer_ssh_pwd, so_file, "./{scp_lib}".format(scp_lib=now_time))
        if not ret:
            print("Scp so to %s is failed, please check your env and input." % developer_ip)
            exit(-1)
    cmd_list = []
    if developer_ssh_user != "root":
        cmd_list.extend([{"type": "cmd",
                         "value": "su root",
                         "secure": False},
                         {"type": "cmd",
                         "value": developer_root_pwd,
                         "secure": True},
                         {"type": "expect",
                          "value": PROMPT}])
    cmd_list.extend([{"type": "cmd",
                      "value": "chmod -R 644 ./{scp_lib}/*".format(scp_lib=now_time),
                      "secure": False},
                     {"type": "expect",
                      "value": PROMPT},
                      {"type": "cmd",
                      "value": "chown -R root:root ./{scp_lib}/*".format(scp_lib=now_time),
                      "secure": False},
                     {"type": "expect",
                      "value": PROMPT},
                     {"type": "cmd",
                      "value": "cp -rdp ./{scp_lib}/* /usr/lib64".format(scp_lib=now_time),
                      "secure": False},
                     {"type": "expect",
                      "value": PROMPT},
                     {"type": "cmd",
                      "value": "rm -rf ./{scp_lib}".format(scp_lib=now_time),
                      "secure": False},
                     {"type": "expect",
                      "value": PROMPT},
                     {"type": "cmd",
                      "value": "ldconfig",
                      "secure": False},
                     {"type": "expect",
                      "value": PROMPT}])
    ret = ssh_to_remote(developer_ssh_user, developer_ip, developer_ssh_port,
                  developer_ssh_pwd, cmd_list)
    if not ret:
        print("Installation is failed.")
        exit(-1)

    print("Adding engine setting in DDK configuration.")
    add_engine_setting(mode, host_engine_settings, device_engine_settings)

    print("Common installation is finished.")


if __name__ == '__main__':
    main()
