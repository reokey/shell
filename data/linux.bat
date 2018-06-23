linux(){
#notice=`curl -s http://www.lu8.win/downloads/frp/gonggao | sed -n 's/gonggao\=//p'`
#versiontime=`curl -s http://www.lu8.win/downloads/frp/gonggao | sed -n 's/versiontime\=//p'`
notice='请注意客户端和服务端版本保持一致！'
versiontime='2017年12月28日'
banquan(){
echo "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
echo "::                                                                          ::"
echo "::      欢迎使用Linux系统的一键脚本.                                        ::"
echo "::          本脚本由 ☆达奚瀚泽☆ 编写,如有疑问请联系本人!                    ::"
echo "::              转载请不要删除本信息!      http://reokey.com                ::"
echo "::                                                                          ::"
echo "::                                                                          ::"
echo "::      作者:☆达奚瀚泽☆        TG:372763861      Ver:$versiontime         ::"
echo "::                                                                          ::"
echo "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
echo ""
echo "$notice"
echo ""   
}

config_server(){
clear
banquan
echo "配置服务器信息："
bind_addr_default="0.0.0.0"
bind_port_default="7000"
bind_udp_port="7001"
kcp_bind_port="7000"
vhost_http_port_default="80"
vhost_https_port_default="443"
dashboard_port_default="7500"
dashboard_user_default="admin"
dashboard_pwd_default="admin"
log_max_days_default="7"
privilege_mode_default="true"
privilege_token_default="frp888"
privilege_allow_ports_default="1-65535"

read -p  "绑定IP地址（默认：$bind_addr_default）:" bind_addr
read -p  "绑定端口（默认：$bind_port_default）:" bind_port
read -p  "HTTP端口（默认：$vhost_http_port_default）:" vhost_http_port
read -p  "HTTPS端口（默认：$vhost_https_port_default）:" vhost_https_port
read -p  "查询连接状态的端口（默认：$dashboard_port_default）:" dashboard_port
read -p  "查询连接状态的用户名（默认：$dashboard_user_default）:" dashboard_user
read -p  "查询连接状态的密码（默认：$dashboard_pwd_default）:" dashboard_pwd
read -p  "日志记录天数（默认：$log_max_days_default）:" log_max_days
read -p  "特权模式开关(true/flase)（默认：$privilege_mode_default）:" privilege_mode
read -p  "特权模式连接密码（默认：$privilege_token_default）:" privilege_token
read -p  "特权模式可用端口范围（默认：$privilege_allow_ports_default）:" privilege_allow_ports

cat > "frps.ini" <<EOF
[common]
bind_addr = ${bind_addr:-$bind_addr_default}
bind_port = ${bind_port:-$bind_port_default}
bind_udp_port = ${bind_udp_port:-$bind_udp_port_default}
kcp_bind_port = ${bind_port:-$bind_port_default}
vhost_http_port = ${vhost_http_port:-$vhost_http_port_default}
vhost_https_port = ${vhost_https_port:-$vhost_https_port_default}
dashboard_port = ${dashboard_port:-$dashboard_port_default}
dashboard_user = ${dashboard_user:-$dashboard_user_default}
dashboard_pwd = ${dashboard_pwd:-$dashboard_pwd_default}
log_max_days = ${log_max_days:-$log_max_days_default}
privilege_mode = ${privilege_mode:-$privilege_mode_default}
privilege_token = ${privilege_token:-$privilege_token_default}
privilege_allow_ports = ${privilege_allow_ports:-$privilege_allow_ports_default}
max_pool_count = 100
log_file = frps.log
log_level = info

EOF

echo ""
read -p  "配置完成，按回车键继续！"
}


run(){
	kill -9 $(pidof frps)
	clear
	banquan
	echo ""
	echo "后台运行frps中..."
	echo ""
	chmod 777 frps
	nohup ./frps -c ./frps.ini & exit | exit
	sleep 3
	cat frps.log
	echo ""
	ver=`./frps -v`
	echo "目前使用的frps程序版本是：$ver"
	echo ""
	read -p  "以上是运行日志，按回车键退出当前显示！"
	echo ""	
	config
}
download(){
			clear
			banquan
			echo ""
			echo "正在为你下载 $os 系统 $version 版frps程序"
			echo ""
			curl -# -o frps http://www.lu8.win/downloads/frp/$os/$version/frps
			if [ $? == 0 ];then				
				run
				else				
				echo "下载失败,请到http://www.lu8.win手动下载程序。"
				echo ""
				echo "请按回车键返回配置"
				read
				config
			fi
}


os_select(){
	clear
	banquan
	echo ""
	echo ""
	echo "请选择你的系统类型："
	echo ""
	echo "1、linux"
	echo "2、linux-arm"
	echo "3、linux-mips"
	echo "4、linux-mipsle"
	echo "5、linux-mips32le"
	echo "6、mac os"
	echo ""
	echo "     备注：直接按回车键返回配置"
	echo ""
	read num
	case "$num" in
		[1] )
			os=linux
			download
		;;
		[2] )
			os=linux-arm
			download
		;;
		[3] )
			os=linux-mips
			download
		;;
		[4] )
			os=linux-mipsle
			download
		;;
		[5] )
			os=linux-mips32le
			download
		;;
		[6] )
			os=mac
			download
		;;
		
		*) echo "选择错误，退出";;
	esac
config
}
	
	

version_select(){

	clear
	banquan
	echo ""
	echo "未检测到该目录下有frps程序，请下载..."
	echo ""
	echo "请选择你要下载的frps的版本："
	echo ""
	echo "     1、V 0.9.3 "
	echo "     2、V 0.14.1"
	echo "     3、V 0.16.1"
	echo ""
	echo "     备注：直接按回车键返回配置"
	echo ""
	read num
	case "$num" in
		[1] )
			
			version=0.9.3
			os_select
			
		;;
		[2] )
		
			version=0.14.1
			os_select
			
		;;
		[3] )
		
			version=latest
			os_select
			
		;;
		*) echo "选择错误，退出";;
	esac
os_select
}

start_up_1(){
			clear
			banquan
			lujing=$(pwd)
			if [ $lujing = "/" ];then
			lujing=""
			fi
			rc_local=$(find / -name rc.local -type f)
			if [ $rc_local = "" ];then
			rc_local=$(find / -name rc -type f)
			fi
			
			echo "以下是前期已经添加过的frps开机启动信息："
			sed -n '/frps/p' "$rc_local"
			sed -i '/此条frps开机启动 由 ☆夢幻煋涳☆ 制作/d' "$rc_local"
			cat "$rc_local" | grep "exit"
			if [ $? -eq 0 ];then	
			sed -i '/exit/i\'$lujing'\/frps \-c '$lujing'\/frps.ini \&	\#此条frps开机启动 由 ☆夢幻煋涳☆ 制作 http\:\/\/www.lu8.win' "$rc_local" 	
			else
			sed -i '$a\'$lujing'\/frps \-c '$lujing'\/frps.ini \&	\#此条frps开机启动 由 ☆夢幻煋涳☆ 制作 http\:\/\/www.lu8.win' "$rc_local"
			fi
			echo ""
			echo "已将 '$lujing/frps -c $lujing/frps.ini &' 添加至开机启动文件 $rc_local"
			echo ""
			chmod 777 $rc_local
			echo "请按回车键返回配置列表"
			read
			config

}

start_up_2(){
			
			clear
			banquan
			lujing=$(pwd)
			if [ $lujing = "/" ];then
			lujing=""
			fi		
			init=$(find / -name init.d -type d|awk 'NR==1{print}')
			
cat > "$init/frps" <<EOF
#!/bin/sh  
START=50
PIDF=\` ps  -A | grep frps | awk '{print \$1}'\`

case "\$1" in
			
start)
		
		rm -rf $lujing/frps.log
		touch $lujing/frps.log
		nohup $lujing/frps -c $lujing/frps.ini &
		echo "frps启动完成"			
		;;

stop)
		kill -3 \$PIDF
		rm -rf $lujing/frps.log
		touch $lujing/frps.log
		echo "frps已经关闭"		
		;;

restart)
		\$0 stop
		sleep 1
		\$0 start
		;;

status)
		
		ps -fe|grep $lujing/frps.ini |grep -v grep
		if [ \$? -ne 0 ]
		then 
		echo "frps未启动"
		else
		echo "frps已启动"
		cat $lujing/frps.log
		fi
		
		
		;;
esac

EOF
			
chmod 777 $init/frps
			
			rc3=$(find / -name rc3.d -type d|awk 'NR==1{print}')
			if [ $rc3 = "" ];then
			rc3=$(find / -name rc.d -type d|awk 'NR==1{print}')
			fi
			
			rm -rf $rc3/S50frps
			sleep 1
			ln -s $init/frps $rc3/S50frps
			
	echo "添加开机启动成功！"		
			

}

config(){
	clear
	banquan
	echo " 请选择你要的操作："
	echo "     1、配置服务端信息"
	echo "     2、配置完成后台启动服务端"
	echo "     3、更换frps程序版本"
	echo "     4、将frps服务端加入开机启动"
	echo "     5、退出"
	echo ""
	echo " 备注：配置过程中若使用默认值，请直接按回车键!"
	echo ""
	read num
	case "$num" in
		[1] )
			config_server
			config			
		;;
		[2] )
			if [ ! -f frps ];then
				version_select
			fi
			
			run
		;;
		[3] )
			kill -9 $(pidof frps)
			rm -rf frps
			version_select
			run			
		;;
		[4] )
			start_up_2
			
			
		;;
		[5] )
			rm $0 & exit			
		;;
		*) echo "选择错误，退出";;
	esac
config
}
config
clear 
}
linux

