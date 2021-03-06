#!/usr/local/bin/python
# -*- coding: iso-8859-1 -*-
import os

#主机定义
HOST_TYPE= \
{
	"Client"			:"C",
	"Gate"				:"G",
	"Zone"				:"Z",
	"Manager"			:"M",
	"Login"				:"L",
	"AIServer"			:"I",
	"Social"			:"S",
	"Admin"				:"D",
	"Daemon"			:"E",
	"History"			:"H",
	"Hotfix"			:"X",
	"Assistant"			:"A",
	"ManagerDBAgent"	:"T",
	"GlobalNpc"			:"N",
	"Stress"			:"Y",
	"Compress"		:"O"
}
#主机关链定义
HOST_LINK= \
{
	"Client"			:["Client","Gate","Stress"],
	"Gate"				:["Gate"],
	"Zone"				:["Zone"],
	"Manager"			:["Manager"],
	"Login"				:["Login","Manager"],
	"AIServer"			:["AIServer"],
	"Social"			:["Social"],
	"Admin"				:["Admin"],
	"Daemon"			:["Daemon"],
	"History"			:["History"],
	"Hotfix"			:["Hotfix"],
	"Assistant"			:["Assistant"],
	"ManagerDBAgent"	:["ManagerDBAgent"],
	"GlobalNpc"			:["GlobalNpc"],
	"Stress"			:["Stress","Gate"],
	"Compress"		:["Compress"],
}
#主机活跃性分类(不活跃的主机与其相关协议号优先保留，仅用于在发生打乱协议操作后绝对不应发生与更新操作事件相关的主机)
HOST_ACTIVITY= \
{
	"Client"				:True,
	"Gate"					:True,
	"Zone"					:True,
	"Manager"				:True,
	"AIServer"				:True,
	"Login"					:True,
	"Social"				:True,
	"Admin"					:False,
	"Daemon"				:False,
	"History"				:True,
	"Hotfix"				:True,
	"Assistant"				:True,
	"ManagerDBAgent"		:False,
	"GlobalNpc"				:True,
	"Stress"				:False,
	"Compress"			:True,
}

