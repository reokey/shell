#!/bin/bash
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin:~/bin
clear



			#############  载入设置  #############
#设置脚本版本
export dx_ver="2018-04-25"
export dx_re=/usr/local/sbin/re
export dx_re_n=/root/re

#设置颜色控制
export dco_r="\033[31m" #红 错误
export dco_g="\033[32m" #绿 菜单
export dco_y="\033[33m" #黄 警告
export dco_p="\033[35m" #紫 提示
export dco_s="\033[36m" #天蓝 正常回显
export dco_t="\033[0m" #关闭颜色
export dco_sb="\033[31m失败\033[0m" #红 失败
export dco_ok="\033[32m成功\033[0m" #绿 成功
export dco_re="\033[31mReOkey \033[36m" #标识符

#设置检测系统变量1
export dos_r=''
export dos_v=''
export dos_b=''
export dos_k=''

#设置检测系统变量2
export SYS_VER=`cat /proc/version`
export sys_debian="Debian"
export SSH_DIR=~/.ssh/
export CON_DIR=/var/spool/cron/crontabs/
export showip=`ip route show | grep -n 'eth0\s*proto\s*kernel\s*scope\s*link\s*src' | sed 's/^.*src //g' | sed s/metric.*$//g`

#创建检测记录文件
re_dir=/data/reokey
mkdir -p ${re_dir}
reokey_file=${re_dir}/result.re
reokey_tree=${re_dir}/tree.re
reokey_key=${re_dir}/keyword.re
if [ -f "$reokey_file" ]; then
	rm -rf ${reokey_file}
fi
if [ -f "$reokey_tree" ]; then
	rm -rf ${reokey_tree}
fi

#检测函数
function CheckDependence(){
FullDependence='0';
for BIN_DEP in `echo "$1" |sed 's/,/\n/g'`
	do
		if [[ -n "$BIN_DEP" ]]; then
			Founded='0';
			for BIN_PATH in `echo "$PATH" |sed 's/:/\n/g'`
				do
					ls $BIN_PATH/$BIN_DEP >/dev/null 2>&1;
					if [ $? == '0' ]; then
						Founded='1';
						break;
					fi
				done
			if [ "$Founded" == '1' ]; then
				echo -en "[${dco_ok}]\t\t";
			else
				FullDependence='1';
				echo -en "[${dco_sb}]\t\t";
			fi
			echo -en "\t${dco_s}$BIN_DEP${dco_t}\n";
		fi
	done
if [ "$FullDependence" == '1' ]; then
	check_dos_r
	if [ "$dos_r" == 'CentOS' ]; then
		echo -ne "\n${dco_sb}! ${dco_s}请执行 '${dco_y}yum install -y${dco_s}' 安装\n\n\n"
	elif [ "$dos_r" == 'Debian' ]; then
		echo -ne "\n${dco_sb}! ${dco_s}请执行 '${dco_y}apt-get install -y${dco_s}' 安装\n\n\n"
	elif [ "$dos_r" == 'Ubuntu' ]; then
		echo -ne "\n${dco_sb}! ${dco_s}请执行 '${dco_y}apt-get install -y${dco_s}' 安装\n\n\n"
	else
		echo -ne "\n${dco_sb}! ${dco_s}请执行 '${dco_y}apt-get install -y${dco_s}' 或者 '${dco_y}yum install -y${dco_s}' 安装\n\n\n"
	fi
	exit 1;
fi
}

function Check_re(){
Check_re='0';
for BIN_DEP in `echo "$1" |sed 's/,/\n/g'`
	do
		if [[ -n "$BIN_DEP" ]]; then
			Founded='0';
			for BIN_PATH in `echo "$PATH" |sed 's/:/\n/g'`
				do
					ls $BIN_PATH/$BIN_DEP >/dev/null 2>&1;
					if [ $? == '0' ]; then
						Founded='1';
						break;
					fi
				done
			if [ "$Founded" == '1' ]; then
				Check_re='1';
				echo -ne "${dco_r}[数据升级]\t";
			else
				Check_re='0';
				echo -ne "${dco_r}[数据安装]\t";
			fi
		fi
	done

if [ -d "$dx_re" ];then
	rm -rf $dx_re
	if [ $? -eq 0 ]; then
		echo -e "${dco_s}存在冲突文件夹 删除成功${dco_t}"
	else
		echo -e "${dco_s}存在冲突文件夹 删除失败${dco_t}"
	fi
else
	if [ ! -f "$dx_re" ];then
		cp -f $0 $dx_re && chmod 777 $dx_re
			if [ $? -eq 0 ]; then
				echo -e "${dco_s} 安装成功${dco_t}"
			else
				echo -e "${dco_s} 安装失败${dco_t}"
			fi
	else
		dx_re_md5=$(md5sum $dx_re|cut -d ' ' -f1)
		dx_re_n_md5=$(md5sum $dx_re_n|cut -d ' ' -f1)
			if [ $dx_re_md5 == $dx_re_n_md5 ]; then
				echo -e "${dco_s}无需更新${dco_t}"
			else
				echo -ne "${dco_s}需要更新${dco_t}"
				cp -f $0 $dx_re && chmod 777 $dx_re
					if [ $? -eq 0 ]; then
						echo -e "${dco_s}\t更新完成${dco_t}"
					else
						echo -e "${dco_s}\t更新失败${dco_t}"
					fi
			fi
	fi
fi
}

#一键更新脚本

update_re(){
	echo -e "${dco_r}[检测版本]${dco_s}\t当前版本\t[${dx_ver}]${dco_t}"
	d_nver=$(wget --no-check-certificate -qO- "https://raw.githubusercontent.com/reokey/shell/master/re"|grep 'dx_ver="'|awk -F "=" '{print $NF}'|sed 's/\"//g'|head -1)
	[[ -z ${d_nver} ]] && echo -e "${dco_r}[检测版本]${dco_s}\t最新\t[无法获取]${dco_t}" && exit 0
		if [[ ${d_nver} != ${dx_ver} ]]; then
			echo -e "${dco_r}[是否下载]${dco_s}\t最新版本\t[${d_nver}]${dco_t}"
			read -n1 -p "" answer
			case $answer in
				n)
					echo -ne "\r     \r"
					echo -e "${dco_r}[数据下载]${dco_s}\t取消\t[${dx_ver}]${dco_t}"
					;;
				*)
					echo -ne "\r     \r"
					echo -e "${dco_r}[数据下载]${dco_s}\t开始下载\t[${d_nver}]${dco_t}"
					rm -rf temp
					wget -q -N -c -O temp --no-check-certificate https://raw.githubusercontent.com/reokey/shell/master/re
					if [ $? -eq 0 ]; then
						
						cp -f temp $dx_re
						chmod +x $dx_re
						echo -e "${dco_r}[下载完成]${dco_s}\t当前版本\t[${d_nver}]${dco_t}"
					else
						echo -e "${dco_r}[下载失败]${dco_s}\t当前版本\t[${dx_ver}]${dco_t}"
					fi
					;;
			esac
		fi
}

			#############  命令开始  #############

#清理命令
clear_command(){
rm -rf dxhz*
}

#错误返回
error_command(){
seconds_left=5
echo -e "${dco_r} 按键非指定命令,返回主菜单.${dco_t}"
while [ $seconds_left -gt 0 ];do
echo -n $seconds_left
sleep 1
seconds_left=$(($seconds_left - 1))
echo -ne "\r     \r"
done
dx_menu
}

#正确返回
correct_command(){
seconds_left=3
echo -e "${dco_y}${seconds_left}秒后返回主菜单.${dco_t}"
while [ $seconds_left -gt 0 ];do
echo -n $seconds_left
sleep 1
seconds_left=$(($seconds_left - 1))
echo -ne "\r     \r"
done
dx_menu
}

#检测系统
check_dos_r()
{
if [ -e /etc/redhat-release ]; then
  dos_r=CentOS
  [ ! -e "$(which lsb_release)" ] && { yum -y install redhat-lsb-core; clear; }
  dos_v=$(lsb_release -sr | awk -F. '{print $1}')
  [ "${dos_v}" == '17' ] && dos_v=7
elif [ -n "$(grep 'Amazon Linux' /etc/issue)" ]; then
  dos_r=CentOS
  dos_v=7
elif [ -n "$(grep 'bian' /etc/issue)" -o "$(lsb_release -is 2>/dev/null)" == "Debian" ]; then
  dos_r=Debian
  [ ! -e "$(which lsb_release)" ] && { apt-get -y update; apt-get -y install lsb-release; clear; }
  dos_v=$(lsb_release -sr | awk -F. '{print $1}')
elif [ -n "$(grep 'Deepin' /etc/issue)" -o "$(lsb_release -is 2>/dev/null)" == "Deepin" ]; then
  dos_r=Debian
  [ ! -e "$(which lsb_release)" ] && { apt-get -y update; apt-get -y install lsb-release; clear; }
  dos_v=8
elif [ -n "$(grep -w 'Kali' /etc/issue)" -o "$(lsb_release -is 2>/dev/null)" == "Kali" ]; then
  dos_r=Debian
  [ ! -e "$(which lsb_release)" ] && { apt-get -y update; apt-get -y install lsb-release; clear; }
  if [ -n "$(grep 'VERSION="2016.*"' /etc/os-release)" ]; then
    dos_v=8
  elif [ -n "$(grep 'VERSION="2017.*"' /etc/os-release)" ]; then
    dos_v=9
  elif [ -n "$(grep 'VERSION="2018.*"' /etc/os-release)" ]; then
    dos_v=9
  fi
elif [ -n "$(grep 'Ubuntu' /etc/issue)" -o "$(lsb_release -is 2>/dev/null)" == "Ubuntu" -o -n "$(grep 'Linux Mint' /etc/issue)" ]; then
  dos_r=Ubuntu
  [ ! -e "$(which lsb_release)" ] && { apt-get -y update; apt-get -y install lsb-release; clear; }
  dos_v=$(lsb_release -sr | awk -F. '{print $1}')
  [ -n "$(grep 'Linux Mint 18' /etc/issue)" ] && dos_v=16
elif [ -n "$(grep 'elementary' /etc/issue)" -o "$(lsb_release -is 2>/dev/null)" == 'elementary' ]; then
  dos_r=Ubuntu
  [ ! -e "$(which lsb_release)" ] && { apt-get -y update; apt-get -y install lsb-release; clear; }
  dos_v=16
else
  echo "${CFAILURE}Does not support this OS, Please contact the author! ${CEND}"
  kill -9 $$
fi

LIBC_YN=$(awk -v A=$(getconf -a | grep GNU_LIBC_VERSION | awk '{print $NF}') -v B=2.14 'BEGIN{print(A>=B)?"0":"1"}')
[ ${LIBC_YN} == '0' ] && GLIBC_FLAG=linux-glibc_214 || GLIBC_FLAG=linux

if uname -m | grep -Eqi "arm"; then
  armplatform="y"
  if uname -m | grep -Eqi "armv7"; then
    TARGET_ARCH="armv7"
  elif uname -m | grep -Eqi "armv8"; then
    TARGET_ARCH="arm64"
  else
    TARGET_ARCH="unknown"
  fi
fi

if [ "$(getconf WORD_BIT)" == "32" ] && [ "$(getconf LONG_BIT)" == "64" ]; then
  dos_b=64
else
  dos_b=32
fi

dos_k=$( uname -r )

THREAD=$(grep 'processor' /proc/cpuinfo | sort -u | wc -l)

# Percona binary
#if [ "${dos_v}" == '5' -o "${dos_v}" == '6' ]; then
#  sslLibVer=ssl098
#elif [ "${dos_v}" == '7' -o "${dos_v}" == '12' ]; then
#  sslLibVer=ssl100
#elif [ "${dos_v}" == '6' -o "${dos_v}" == '8' -o "${dos_v}" == '14' ]; then
#  sslLibVer=ssl101
#elif [ "${dos_v}" == '7' -o "${dos_v}" == '9' -o ${dos_v} -ge 16 >/dev/null 2>&1 ]; then
#  sslLibVer=ssl102
#else
#  sslLibVer=unknown
#fi
}

check_fun()
{
	# release information
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}发行版本信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/*-release >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}发行版本信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}发行版本信息 ...... ${dco_sb}"
	fi

	# ifconfig
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}网络地址信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	ip address show >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}网络地址信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}网络地址信息 ...... ${dco_sb}"
	fi

	# arp
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}A网络缓冲信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	arp -v >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}网络缓冲信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}网络缓冲信息 ...... ${dco_sb}"
	fi

	# route
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}路由信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	route -v >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}路由网关信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}路由网关信息 ...... ${dco_sb}"
	fi

	# /etc/passwd
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}密码文件信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	if [ "$userinfo" ]
	then
		echo -e "\n$userinfo" >> $reokey_file 2>&1
	else
		:
	fi
	hashesinpasswd=`grep -v '^[^:]*:[x]' /etc/passwd 2>/dev/null`
	if [ "$hashesinpasswd" ]
	then
		echo -e "\e[00;33mIt looks like we have password hashes in 密码文件信息!\e[00m\n$hashesinpasswd" >> $reokey_file 2>&1
	else
		:
	fi
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}密码文件信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}密码文件信息 ...... ${dco_sb}"
	fi

	# /etc/shadow
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}系统提交文件信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/shadow >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}提交文件信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}提交文件信息 ...... ${dco_sb}"
	fi

	# /etc/sudoers
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}系统提交文件信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/sudoers 2>/dev/null | grep -v -e '^$' | grep -v "#" >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}密码镜像文件 ...... ${dco_ok}"
	else
		echo -e "${dco_re}密码镜像文件 ...... ${dco_sb}"
	fi

	# password policy information
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}密码策略信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/login.defs 2>/dev/null | grep -v -e '^$' | grep -v "#" >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}密码策略信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}密码策略信息 ...... ${dco_sb}"
	fi

	# /root
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}检查根目录${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	ls -ahl /root/ >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}检查根目录信 ...... ${dco_ok}"
	else
		echo -e "${dco_re}检查根目录信 ...... ${dco_sb}"
	fi

	# netstat
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}网络状态${dco_t}" >> $reokey_file
	echo -e "${dco_s}------------------TCP---PORT------------------------${dco_t}" >> $reokey_file
	netstat -ReOkeyp >> $reokey_file 2>&1
	echo -e "${dco_s}------------------UDP---PORT------------------------${dco_t}" >> $reokey_file
	netstat -anup >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}网络状态 ...... ${dco_ok}"
	else
		echo -e "${dco_re}网络状态 ...... ${dco_sb}"
	fi

	# process
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}进程信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	ps aux >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}进程信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}进程信息 ...... ${dco_sb}"
	fi

	# services
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}服务信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/services 2>/dev/null | grep -v "#" >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}服务信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}服务信息 ...... ${dco_sb}"
	fi

	# bash env
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}Bash Env${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}当前用户 ${dco_t}" >> $reokey_file
	echo -e "${dco_s}-----------------------${dco_t}" >> $reokey_file
	envinfo=`env 2>/dev/null | grep -v 'LS_COLORS' 2>/dev/null`
	if [ "$envinfo" ]
	then
		echo -e "\n$envinfo" >> $reokey_file 2>&1
		echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	else
		:
	fi
	cat ~/.bashrc 2>/dev/null | grep -v -e '^$' | grep -v "#" >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo $PATH >> $reokey_file 2>&1
	echo -e "${dco_s}-----------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}环境变量 ${dco_t}" >> $reokey_file
	echo -e "${dco_s}-----------------------${dco_t}" >> $reokey_file
	cat /etc/profile 2>/dev/null | grep -v -e '^$' | grep -v "#" >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}环境变量 ...... ${dco_ok}"
	else
		echo -e "${dco_re}环境变量 ...... ${dco_sb}"
	fi

	# command history
	HISTFILE=~/.bash_history
	export HISTTIMEFORMAT="%Y-%m-%d:%H-%M-%S:"`whoami`": "
	set -o history
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}命令历史${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	history >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat $HISTFILE >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}命令历史 ...... ${dco_ok}"
	else
		echo -e "${dco_re}命令历史 ...... ${dco_sb}"
	fi

	# user login info
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}登录信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	w >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	last >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	lastlog >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}登录信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}登录信息 ...... ${dco_sb}"
	fi

	# hosts
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}主机信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	find /etc /home -type f \( -name "*.rhosts" -o -name "*.equiv" \) >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/hosts >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}主机信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}主机信息 ...... ${dco_sb}"
	fi

	# fstab
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}安装信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/fstab >> $reokey_file 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}挂载信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}挂载信息 ...... ${dco_sb}"
	fi

	# ssh authkey config
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}SSH密钥设置信息${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	sshfiles=`find / \( -name "id_dsa*" -o -name "id_rsa*" -o -name "known_hosts" -o -name "authorized_hosts" -o -name "authorized_keys" \) -exec ls -la {} 2>/dev/null \;`
	if [ "$sshfiles" ]; then
		echo -e "\e[00;31m在下列位置找到的SSH密钥/主机信息：\e[00m\n$sshfiles" >> $reokey_file 2>&1
		echo -e "\n" >> $reokey_file 2>&1
	else
		:
	fi
	if [ -d "${SSH_DIR}" ]
	then
		for i in `ls -1 ${SSH_DIR} >> $reokey_file 2>&1`
		do
			cat ${SSH_DIR}${i} >> $reokey_file 2>&1
		done
		if [[ $? -eq 0 ]]
		then
			echo >> $reokey_file
			echo -e "${dco_re}SSH 密钥 ...... ${dco_ok}"
		else
			echo -e "${dco_re}SSH 密钥 ...... ${dco_sb}"
		fi
	else
		echo -e "${dco_re}SSH 密钥 ...... ${dco_sb}"
		echo -e "${dco_re}.ssh 没有这样的文件或目录${dco_t}" >> $reokey_file
	fi

	# crontab
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	echo -e "${dco_g}定时任务设置${dco_t}" >> $reokey_file
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	ls -al /etc/cron* >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cat /etc/rc.local >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	cut -d ":" -f 1 /etc/passwd | xargs -n1 crontab -l -u >> $reokey_file 2>&1
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_file
	if [ -d "${CON_DIR}" ]
	then
		for i in `ls -1 ${CON_DIR} >> $reokey_file 2>&1`
		do
			cat ${CON_DIR}${i} >> $reokey_file 2>&1
		done
		if [[ $? -eq 0 ]]
		then
			echo >> $reokey_file
			echo -e "${dco_re}定时任务 ...... ${dco_ok}"
		else
			echo -e "${dco_re}定时任务 ...... ${dco_sb}"
		fi
	else
		echo -e "${dco_re}定时任务 ...... ${dco_sb}"
		echo -e "${dco_re} 没有这样的文件或目录${dco_t}" >> $reokey_file
	fi
}

check_tree()
{
# sensitive data
a_dir=("/etc/" "/opt/" "/var/" "/home/" "/root/" "/usr/")
# a_file=("redis.conf" "mongodb.conf" "server.xml" "vsftpd.conf" "ldap.conf" "nginx.conf" "apache2.conf" "smb.conf")
if command -v tree > /dev/null 2>&1; then
	if [[ -n $1 ]]; then
		for i in ${a_dir[@]}
		do
			echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
			echo -e "${dco_g}${i}${dco_t}" >> $reokey_tree
			echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
			tree -L $1 ${i} >> $reokey_tree 2>&1
		done
		if [[ $? -eq 0 ]]
		then
			echo >> $reokey_file
			echo -e "${dco_re}目录结构信息 ...... ${dco_ok}"
		else
			echo -e "${dco_re}目录结构信息 ...... ${dco_sb}"
		fi
	else
		for i in ${a_dir[@]}
		do
			echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
			echo -e "${dco_g}${i}${dco_t}" >> $reokey_tree
			echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
			tree ${i} >> $reokey_tree 2>&1
		done
		if [[ $? -eq 0 ]]
		then
			echo >> $reokey_file
			echo -e "${dco_re}目录结构信息 ...... ${dco_ok}"
		else
			echo -e "${dco_re}目录结构信息 ...... ${dco_sb}"
		fi
	fi
else
	for s in ${a_dir[@]}
	do
		echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
		echo -e "${dco_g}${dco_s}${dco_t}" >> $reokey_tree
		echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_tree
		ls -R -t -s -a ${dco_s} >> tree.dxl 2>&1
	done
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_file
		echo -e "${dco_re}目录结构信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}目录结构信息 ...... ${dco_sb}"
	fi
fi
}

check_key()
{
if [[ -n $1 ]]
then
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_key
	echo -e "${dco_g}关键信息信息${dco_t}" >> $reokey_key
	echo -e "${dco_s}----------------------------------------------------${dco_t}" >> $reokey_key
	find / -maxdepth 4 -name *.conf -type f -exec grep -Hn $1 {} \; >> $reokey_key 2>&1
	find / -maxdepth 4 -name *.ini -type f -exec grep -Hn $1 {} \; >> $reokey_key 2>&1
	find / -maxdepth 4 -name *.log -type f -exec grep -Hn $1 {} \; >> $reokey_key 2>&1
	if [[ $? -eq 0 ]]
	then
		echo >> $reokey_key
		echo -e "${dco_re}关键信息信息 ...... ${dco_ok}"
	else
		echo -e "${dco_re}关键信息信息 ...... ${dco_sb}"
	fi
else
	:
fi
}



#检测软件
check_Python()
{
V1=2
V2=6
V3=5
U_V1=`python -V 2>&1|awk '{print $2}'|awk -F '.' '{print $1}'`
U_V2=`python -V 2>&1|awk '{print $2}'|awk -F '.' '{print $2}'`
U_V3=`python -V 2>&1|awk '{print $2}'|awk -F '.' '{print $3}'`
#	if [ $U_V1 -lt $V1 ];then
#		echo 'Your python version is not OK!(1)'
#		exit 1
#	elif [ $U_V1 -eq $V1 ];then
#		if [ $U_V2 -lt $V2 ];then
#			echo 'Your python version is not OK!(2)'
#			exit 1
#		elif [ $U_V2 -eq $V2 ];then
#			if [ $U_V3 -lt $V3 ];then
#				echo 'Your python version is not OK!(3)'
#				exit 1
#			fi
#		fi
#	fi
#	echo Your python version is OK!
}


check_v01()
{
	v01=wget
	x=`rpm -qa | grep $v01`
	if [ `rpm -qa | grep $v01 |wc -l` -ne 0 ];then
	s01="$v01       - $st1"
	else
	s01="$v01       - $st2"
	fi
}
check_v02()
{
	v02=curl
	x=`rpm -qa | grep $v02`
	if [ `rpm -qa | grep $v02 |wc -l` -ne 0 ];then
	s02="$v02       - $st1"
	else
	s02="$v02       - $st2"
	fi
}
check_v03()
{
	v03=git
	x=`rpm -qa | grep $v03`
	if [ `rpm -qa | grep $v03 |wc -l` -ne 0 ];then
	s03="$v03        - $st1"
	else
	s03="$v03        - $st2"
	fi
}
check_v04()
{
	v04=pip
	x=`rpm -qa | grep $v04`
	if [ `rpm -qa | grep $v04 |wc -l` -ne 0 ];then
	s04="$v04        - $st1"
	else
	s04="$v04        - $st2"
	fi
}
check_v05()
{
	v05=gcc
	x=`rpm -qa | grep $v05`
	if [ `rpm -qa | grep $v05 |wc -l` -ne 0 ];then
	s05="$v05        - $st1"
	else
	s05="$v05        - $st2"
	fi
}
check_v06()
{
	v06=make
	x=`rpm -qa | grep $v06`
	if [ `rpm -qa | grep $v06 |wc -l` -ne 0 ];then
	s06="$v06       - $st1"
	else
	s06="$v06       - $st2"
	fi
}
check_v07()
{
	v07=screen
	x=`rpm -qa | grep $v07`
	if [ `rpm -qa | grep $v07 |wc -l` -ne 0 ];then
	s07="$v07     - $st1"
	else
	s07="$v07     - $st2"
	fi
}

#脚本必备组件
reokey_module(){
	clear
	menu_ver
	echo -e "${dco_g}───────────────────────────────── 一 键 必 备  ─────────────────────────────────${dco_t}"
	#检测软件
	st1=已安装
	st2=未装或未被检测
	check_v01
	echo -e "${dco_g}  $s01 ${dco_t}"
	check_v02
	echo -e "${dco_g}  $s02 ${dco_t}"
	check_v03
	echo -e "${dco_g}  $s03 ${dco_t}"
	check_v04
	echo -e "${dco_g}  $s04 ${dco_t}"
	check_v05
	echo -e "${dco_g}  $s05 ${dco_t}"
	check_v06
	echo -e "${dco_g}  $s06 ${dco_t}"
	check_v07
	echo -e "${dco_g}  $s07 ${dco_t}"
	echo
	opst="检测当前系统为$dos_r $dos_b位 请按任意键继续... 主菜单+ / 退出-"
	if [[ -f /etc/redhat-release ]]; then
		module_centos
	elif cat /etc/issue | grep -q -E -i "debian"; then
		module_debian
	elif cat /etc/issue | grep -q -E -i "ubuntu"; then
		module_ubuntu
	elif cat /etc/issue | grep -q -E -i "centos|red hat|redhat"; then
		module_centos
	elif cat /proc/version | grep -q -E -i "debian"; then
		module_debian
	elif cat /proc/version | grep -q -E -i "ubuntu"; then
		module_ubuntu
	elif cat /proc/version | grep -q -E -i "centos|red hat|redhat"; then
		module_centos
	fi
}

#脚本必备组件centos
module_centos(){
	echo -e "${dco_g}当前系统为 Centos $dos_b位 请按任意键继续...主菜单+ / 退出-${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		*)
			yum -y install reinstall gcc gcc-c++ bzip2 make vixie-cron flex ncurses-devel wget patch ntp libxml2 libxml2-devel libevent m4 autoconf zip unzip libjpeg libjpeg-devel gd gd-devel freetype freetype-devel libpng libpng-devel openssl openssl-devel file libtool libtool-libs gmp-devel pspell-devel parted zlib perl mod_perl-devel apr-util ftp readline-devel readline-devel apr apr-util curl-devel screen
			reokey_module
			;;
	esac
	exit 0
}

#脚本必备组件debian
module_debian(){
	echo -e "${dco_g}当前系统为 Debian $dos_b位 请按任意键继续...主菜单+ / 退出-${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		*)
			apt-get -y install gcc g++ ssh make bzip2 flex vim bison libtool libncurses5-dev libncurses5 libncurses5-dev libncurses5-dev libpcrecpp0 patch ntpdate openssl libssl-dev build-essential file gawk binutils parted zip unzip libperl-dev perl ftp libreadline-dev screen
			reokey_module
			;;
	esac
	exit 0
}

#脚本必备组件ubuntu
module_ubuntu(){
	echo -e "${dco_g}当前系统为 Ubuntu $dos_b位 请按任意键继续...主菜单+ / 退出-${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		*)
			apt-get -y install gcc g++ ssh make bzip2 flex vim bison libtool libncurses5-dev libncurses5 libncurses5-dev libncurses5-dev libpcrecpp0 patch ntpdate openssl libssl-dev build-essential file gawk binutils parted zip unzip libperl-dev perl ftp libreadline-dev screen
			reokey_module
			;;
	esac
	exit 0
}

#一键更换系统
reokey_os(){
	title=" 一键更换系统 "
	author="萌咖"
	source="https://moeclub.org/2018/04/03/603/?v=244"
	release=未选
	framework=未选
	version=未选
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}请选择系统发行版本 [1]Centos [2]Debian [3]Ubuntu [4]Windows:${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		1)
			release=-c
			reokey_os_centos
			;;
		2)
			release=-d
			reokey_os_debian
			;;
		3)
			release=-u
			reokey_os_ubuntu
			;;
		4)
			release=-w
			reokey_os_windows
			;;
		*)
			reokey_os
			;;
	esac
}
#一键更换系统-选择构架
reokey_os_centos(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}继续选择系统构架 [1]64位 [2]32位${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		1)
			framework=64
			reokey_os_centos_64
			;;
		2)
			framework=32
			reokey_os_centos_32
			;;
		*)
			reokey_os_centos
			;;
	esac
}
reokey_os_debian(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}继续选择系统构架 [1]64位 [2]32位${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		1)
			framework=64
			reokey_os_debian_64
			;;
		2)
			framework=32
			reokey_os_debian_32
			;;
		*)
			reokey_os_debian
			;;
	esac
}
reokey_os_ubuntu(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}继续选择系统构架 [1]64位 [2]32位${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		1)
			framework=64
			reokey_os_ubuntu_64
			;;
		2)
			framework=32
			reokey_os_ubuntu_32
			;;
		*)
			reokey_os_ubuntu
			;;
	esac
}
reokey_os_windows(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}继续选择系统构架 [1]64位 [2]32位${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		1)
			framework=64
			reokey_os_windows_64
			;;
		2)
			framework=32
			reokey_os_windows_32
			;;
		*)
			reokey_os_windows
			;;
	esac
}
#一键更换系统-选择版本
reokey_os_centos_64(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Centos6.8 [2]Centos5.8${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=6.8
				reokey_os_install
				;;
			2)
				version=5.8
				reokey_os_install
				;;
			*)
				reokey_os_centos_64
				;;
		esac
}
reokey_os_centos_32(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Centos6.8 [2]Centos5.8${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=6.8
				reokey_os_install
				;;
			2)
				version=5.8
				reokey_os_install
				;;
			*)
				reokey_os_centos_32
				;;
		esac
}
reokey_os_debian_64(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Debian7 [2]Debian8 [3]Debian9${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=7
				reokey_os_install
				;;
			2)
				version=8
				reokey_os_install
				;;
			3)
				version=9
				reokey_os_install
				;;
			*)
				reokey_os_debian_64
				;;
		esac
}
reokey_os_debian_32(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Debian7 [2]Debian8 [3]Debian9${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=7
				reokey_os_install
				;;
			2)
				version=8
				reokey_os_install
				;;
			3)
				version=9
				reokey_os_install
				;;
			*)
				reokey_os_debian_32
				;;
		esac
}
reokey_os_ubuntu_64(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Ubuntu 18.04 [2]Ubuntu 16.04 [3]Ubuntu 14.04${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=18.04
				reokey_os_install
				;;
			2)
				version=16.04
				reokey_os_install
				;;
			3)
				version=14.04
				reokey_os_install
				;;
			*)
				reokey_os_ubuntu_64
				;;
		esac
}
reokey_os_ubuntu_32(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Ubuntu 18.04 [2]Ubuntu 16.04 [3]Ubuntu 14.04${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=18.04
				reokey_os_install
				;;
			2)
				version=16.04
				reokey_os_install
				;;
			3)
				version=14.04
				reokey_os_install
				;;
			*)
				reokey_os_ubuntu_32
				;;
		esac
}
reokey_os_windows_64(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Windows Server 2012 [2]Windows Server 2016${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=2012
				reokey_os_install
				;;
			2)
				version=2016
				reokey_os_install
				;;
			*)
				reokey_os_windows_64
				;;
		esac
}
reokey_os_windows_32(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo
	echo -e "${dco_y}最后敲定系统版本 [1]Windows Server 2012 [2]Windows Server 2016${dco_t}"
		read -n1 -p "" answer
		case $answer in
			+)
				dx_menu
				;;
			-)
				exit
				;;
			1)
				version=2003
				reokey_os_install
				;;
			2)
				version=2008
				reokey_os_install
				;;
			*)
				reokey_os_windows_32
				;;
		esac
}
#一键更换系统-开始安装
reokey_os_install(){
	clear
	menu_ver
	menu_author
	echo
	echo -e "${dco_g}目前参数:${dco_t}"
	echo -e "${dco_g}系统 $release  构架 $framework  版本 $version${dco_t}"
	echo -e "${dco_y}已经确定全部参数,按[任意键]开始安装,密码[Vicer]${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		*)
			curl -# -o dxhz7 https://raw.githubusercontent.com/reokey/shell/master/data/dxhz7
			sh dxhz7
			bash dxhz7 $release $version -v $framework -a
			;;
	esac
}

#一键升级内核
reokey_kernel(){
	clear
	menu_ver
	echo -e "${dco_y}升级内核组件[ ${d_ver} ]!${dco_t}"
	echo -e "${dco_y}———————————————————————————${dco_t}"
	echo -e "${dco_y} 7.  一键安装最新内核开启BBR${dco_t}"
	echo -e "${dco_y} 8.  卸载旧kernel及相关${dco_t}"
	echo -e "${dco_y} 9.  查看当前kernel信息${dco_t}"
	echo -e "${dco_y}———————————————————————————${dco_t}"
	echo
	echo
	echo
	echo -e "${dco_g} 请输入 [7-9]:${dco_t}"
	read -n1 -p "" answer
	case $answer in
		+)
			dx_menu
			;;
		-)
			exit
			;;
		7)
			curl -# -o dxhz8 https://raw.githubusercontent.com/reokey/shell/master/data/dxhz8
			sh dxhz8
			;;
		8)
			yum remove *kernel* -y
			yum --enablerepo=elrepo-kernel install kernel-ml-headers kernel-ml-tools kernel-ml-tools-libs -y
			yum install dracut-kernel -y
			reokey_kernel
			;;
		9)
			echo -ne "\r     \r"
			clear
			menu_ver
			echo -e "${dco_y}当前kernel信息${dco_t}"
			echo
			rpm -qa |grep kernel
			echo
			echo -e "${dco_g} [任意键]返回:${dco_t}"
				read -n1 -p "" answer
				case $answer in
					*)
					reokey_kernel
					;;
				esac
			;;
		*)
		error_command
		;;
	esac
}


			#############  菜单开始  #############
#脚本介绍
menu_ver(){
	menu_help_tips
	menu_banner
	echo && echo -e "  ${dco_g}版本信息待添加\t\t\t\t[更新日期 ${dx_ver}]${dco_t}"
}

#菜单头部
menu_banner(){
	clear
	echo -e "${dco_g}───────────────────────────── Linux Server Script ─────────────────────────────${dco_t}"
	echo -e "${dco_g}   ______                 _                   ${dco_t}"
	echo -e "${dco_g}  (_____ \               | |                  ${dco_t}"
	echo -e "${dco_g}   _____) ) _____   ___  | |  _  _____  _   _ ${dco_t}"
	echo -e "${dco_g}  |  __  / | ___ | / _ \ | |_/ )| ___ || | | |${dco_t}"
	echo -e "${dco_g}  | |  \ \ | ____|| |_| ||  _ ( | ____|| |_| |${dco_t}"
	echo -e "${dco_g}  |_|   |_||_____) \___/ |_| \_)|_____) \__  |${dco_t}"
	echo -e "${dco_g}                                       (____/ ${dco_t}"
}

#菜单帮助
menu_help(){
	echo && echo -e "  ${dco_g}达奚一键安装管理脚本\t\t\t\t[Ver ${dx_ver}]${dco_t}"
	echo
	echo -ne "${dco_y}说明:\t [作用]...\t命令示例\t\t[说明]....${dco_t}\n\n\t ${dco_g}[帮助]\t\tre help\t\t\t[项目]\n\t [清单]${dco_t}\t\t${dco_g}re list\t\t\t[无]\n\t [管理]\t\tre manage\t\t[授权]\n\t [脚本]\t\tre ok\t\t\t[项目]\n\t [扫描]\t\tre scan\t\t\t[项目]\n\t [版本]\t\tre ver\t\t\t[项目]${dco_t}\n\n"
	update_re
	Check_re re;
}

#菜单引导
menu_gui(){
	clear
	if [[ -z "$dos_r" ]]; then
		check_dos_r
	fi
	if [[ -z "$dos_v" ]]; then
		check_dos_r
	fi
	if [[ -z "$dos_b" ]]; then
		check_dos_r
	fi
	if [[ -z "$dos_k" ]]; then
		check_dos_r
	fi
	menu_command
}

#菜单命令
menu_gui(){
	echo && echo -e "  ${dco_g}选择菜单待添加\t\t[Ver ${dx_ver}]${dco_t}"
	echo
	echo -ne "${dco_y}说明:\t [作用]...\t命令示例\t\t[参数]...."
}


			#############  脚本开始  #############



while :; do
	[ -z "$1" ] && menu_help && exit 0 #无参数跳转到 菜单帮助 否则执行参数版
	case "$1" in
	check)
		menu_banner;
		CheckDependence wget,awk,grep,sed,cut,cat,cpio,gzip,find,dirname,basename,file,xz,git;
		exit 0
	;;
	help)
		menu_banner
		menu_help;
		exit 0
	;;
	list)
		menu_banner
		IPLIST=$2; shift 2
	;;
	manage)
		menu_banner
		manage;
		exit 0
	;;
	ok)
		menu_banner
		menu_gui;
		exit 0
	;;
	scan)
		menu_banner
		check_fun
		check_tree
		check_key
		check_dos_r;
		exit 0
	;;
	ver)
		menu_banner
		menu_ver
		exit 0
	;;
	-ssh_port)
		ssh_port=$2; shift 2
		[ ${ssh_port} -eq 22 >/dev/null 2>&1 -o ${ssh_port} -gt 1024 >/dev/null 2>&1 -a ${ssh_port} -lt 65535 >/dev/null 2>&1 ] || { echo "${CWARNING}ssh_port input error! Input range: 22,1025~65534${CEND}"; exit 1; }
	;;
	--)
		shift
	;;
	*)
		menu_help;
		echo -e "${dco_sb}\t${dco_y}未知命令 [${1}]${dco_t}"
		exit 1
	;;
	esac
done
exit 0