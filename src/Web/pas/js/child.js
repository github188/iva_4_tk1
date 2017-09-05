/**
 * 子页面公用功能
 * @returns {Boolean|Object} [[Description]]
 */
var pageSize = 10;

function addEvent(obj, name, func) {
    if (obj.attachEvent) {
        obj.attachEvent("on" + name, func);
    } else {
        obj.addEventListener(name, func, false);
    }
}

var eventParams = {};

var child = function () {
    return {
        addEvent: function () {
            //子页面中子页面的切换
            $('.lanmubox li a').bind('click', function () {
                var url = $(this).attr('data-url') + '.html?timestamp=' + new Date().getTime();
                $('.lanmu-active').removeClass('lanmu-active');
                $(this).parent().addClass('lanmu-active');
                //   $("#mainContent").empty();
                $("#mainContent").load(url);
            });

            $(".tab_content").hide(); //Hide all content
            $("ul.jh-tab li:first").addClass("active").show(); //Activate first tab
            $(".tab_content:first").show(); //Show first tab content

            //On Click Event
            $("ul.jh-tab li").click(function () {
                $("ul.jh-tab li").removeClass("active"); //Remove any "active" class
                $(this).addClass("active"); //Add "active" class to selected tab
                $(".tab_content").hide(); //Hide all tab content
                var activeTab = $(this).find("a").attr("href"); //Find the rel attribute value to identify the active tab + content
                $(activeTab).fadeIn(); //Fade in the active content
                return false;
            });

        },
        getWeekDay: function () {
            var now = new Date();
            var day = now.getDay();
            return day;
        },
        showDay: function () {
            var day = child.getWeekDay();
            $('.jihualuzhi').find('li').each(function () {
                if ($(this).attr('aweek') == day) {
                    $(this).addClass('active');
                } else {
                    $(this).removeClass('active');
                }
            });
            $('.tab_content').hide();
            var celueId = '#p-tabs-week' + day;
            $(celueId).show();
        },
        logout: function () {
            var store = window.localStorage;
            store.clear();
            location.href = "login.html";
        },
        initDefaultTime: function () {
            var now = new Date(); //获取系统日期，如Sat Jul 29 08:24:48 UTC+0800 2012    
            var yy = now.getFullYear(); //截取年，即2012    
            var MM = now.getMonth() + 1; //截取月，即07    
            var dd = now.getDate(); //截取日，即29 
            if (MM < 10) { //目的是构造2012-12-04这样的日期
                MM = "0" + MM;
            }
            if (dd < 10) {
                dd = "0" + dd;
            }
            var hh = now.getHours(); //截取小时，即8    
            var mm = now.getMinutes(); //截取分钟，即34    
            var ss = now.getTime() % 60000;
            //获取时间，因为系统中时间是以毫秒计算的， 所以秒要通过余60000得到。    
            ss = (ss - (ss % 1000)) / 1000; //然后，将得到的毫秒数再处理成秒    
            if (hh < 10) hh = '0' + hh; //字符串    
            if (mm < 10) mm = '0' + mm; //字符串    
            if (ss < 10) ss = '0' + ss; //字符串    
            //  var defaultStartTime = yy + "-" + MM + "-" + dd + " " + "00:00:00";
            var defaultStartTime = yy + "-" + MM + "-" + dd + " ";
            //   var defaultEndTime = yy + "-" + MM + "-" + dd + " " + "23:59:59";
            //var defaultEndTime = yy + "-" + MM + "-" + dd + " " + hh+":"+mm+":"+ss;

            return defaultStartTime;
        }
    }
}();
var pluginVer = 2.26;
var g_bPlay = false;
var bPluginInit = false;
var alarmLen = 0;

//网络配置
var wlpz = function () {
    return {
        addEvent: function () {
            child.addEvent();
            wlpz.init();
            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });

            $("#message").validationEngine("attach", {
                promptPosition: "centerRight",
                scroll: false
            });
            $('.save-btn').click(function () {
                $('.save-successful').hide();
                if (!$("#message").validationEngine("validate")) {
                    return;
                }
                wlpz.setNetInterfaces();
                wlpz.setNetDNS();
                //    console.log(localStorage.isSupport4G);
                if (localStorage.isSupport4G) {
                    wlpz.setNetwork4G();
                }
                if (localStorage.isSupportWifi) {
                    wlpz.setNetworkWlanHot();
                }
            });
        },
        init: function () {
            wlpz.getNetCapabilities();
            wlpz.getNetInterfaces();
        },
        getNetworkWlanHot: function () {
            $.ajax({
                url: "/ISAPI/System/Network/WlanHot",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功 
                        var list = data.wlanhot;
                        if (list.enable) {
                            $("#wlanhot").addClass('a-offon-click');
                        } else {
                            $("#wlanhot").removeClass('a-offon-click');
                        }
                        $("#netSSID").val(list.ssid);
                        var b = new Base64();
                        var encodePwd = b.decode(list.password);

                        $("#WAP2PSK").val(encodePwd);
                        $("#maxclient").val(list.maxclient);
                        $("#frequency").val(list.frequency);
                    } else {

                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNetworkWlanHot: function () {
            var param = {};
            var con = {};
            if ($("#wlanhot").hasClass('a-offon-click')) {
                con.enable = true;
            } else {
                con.enable = false;
            }
            con.ssid = $('#netSSID').val();
            var b = new Base64();
            var encodePwd = b.encode($('#WAP2PSK').val());

            con.password = encodePwd;
            con.maxclient = parseInt($('#maxclient').val());
            con.frequency = parseInt($('#frequency').val());
            param.wlanhot = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Network/WlanHot",
                type: "PUT",
                cache: false,
                data: param,
                datatype: 'json',
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {

                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNetwork4G: function () {
            var param = {};
            var con = {};
            if ($("#fourD").hasClass('a-offon-click')) {
                con.enable = true;
            } else {
                con.enable = false;
            }
            con.telid = parseInt($('#telidSelect').val());
            param.fourG = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Network/4G",
                type: "PUT",
                cache: false,
                data: param,
                datatype: 'json',
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {

                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNetwork4G: function () {
            $.ajax({
                url: "/ISAPI/System/Network/4G",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功 
                        if (data.fourG.enable) {
                            $("#fourD").addClass('a-offon-click');
                        } else {
                            $("#fourD").removeClass('a-offon-click');
                        };
                        $("#telidSelect").val(data.fourG.telid);
                    } else {

                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNetCapabilities: function () {
            $.ajax({
                url: "/ISAPI/System/Network/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        var list = data.networkCap;
                        localStorage.maxInterfaceNum = list.maxInterfaceNum;

                        localStorage.isSupportWifi = list.isSupportWifi;
                        if (list.isSupportWifi) {
                            wlpz.getNetworkWlanHot();
                            $("#netSSID,#WAP2PSK,#maxclient,#frequency").attr("disabled", false);
                        } else {
                            $("#wlanhot").removeClass('a-offon');
                            $("#netSSID,#WAP2PSK,#maxclient,#frequency").attr("disabled", true);
                        }
                        localStorage.isSupport4G = list.isSupport4G;
                        if (list.isSupport4G) {
                            var sel = list.support4GTelecom; //支持的
                            $("#telidSelect").empty();
                            var str = "";
                            for (var i = 0; i < sel.length; i++) {
                                str += "<option value=" + sel[i].telid + ">" + sel[i].telname + "</option>";
                            }
                            $("#telidSelect").append(str);

                            wlpz.getNetwork4G();
                        } else {
                            $("#fourD").removeClass('a-offon');
                        }
                    } else {

                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNetDNS: function (str) {
            $.ajax({
                url: "/ISAPI/System/Network/DNS",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        var list = data.dns;
                        str += '<tr><th>DNS</th><td><div class="wlpz-item">首选DNS : <input id="fDNS" type="text" class="input-default validate[custom[ipv4]]" value="' + list.main + '" /></div>' + ' <div class="wlpz-item">备选DNS : <input id="sDNS" type="text" class="input-default validate[custom[ipv4]]" value="' + list.standby + '" /></div></td></tr>';
                        $(".list-table").append(str);
                    } else {

                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNetDNS: function (str) {
            var param = {};
            var con = {};
            con.main = $('#fDNS').val();
            con.standby = $('#sDNS').val();
            param.dns = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Network/DNS",
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        $('.save-successful').show();
                    } else {

                    }
                },
                error: function (data) {
                    alert(data)
                }
            });
        },
        getNetInterfaces: function () {
            $.ajax({
                url: "/ISAPI/System/Network/Interfaces",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        var list = data.interfaces;
                        var len = localStorage.maxInterfaceNum;
                        $(".list-table").empty();
                        var str = "";
                        for (var i = 0; i < len; i++) {
                            var netInfo = list[i].networkinfo;
                            str += '<tr class="netInfo"><th>Lan' + (i + 1) + '配置</th>' + '<td><div class="wlpz-item">IPV4地址：<input type="text" class="address input-default validate[custom[ipv4],required]" data-name=' + list[i].ifname + ' value="' + netInfo.address + '" /></div>' + '<div class="wlpz-item">子网掩码：  <input type="text" class="netmask input-default validate[custom[ipv4],required]" value="' + netInfo.netmask + '" /></div>' + ' <div class="wlpz-item">网关： <input type="text" class="gateway input-default validate[custom[ipv4],required]" value="' + netInfo.gateway + '" />  </div>' +
                                '<div class="wlpz-item">MAC地址： <input type="text" class="input-default" value="' + list[i].mac + '" disabled/></div></td></tr>';
                        };

                        wlpz.getNetDNS(str);
                    } else {

                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNetInterfaces: function () {
            var paramPro = {};
            var param = [];
            var mac = '';

            $('.list-table').find('.netInfo').each(function () {
                var con = {};
                var o = {};
                o.address = $(this).find('.address').val();
                o.netmask = $(this).find('.netmask').val();
                o.gateway = $(this).find('.gateway').val();
                con.networkinfo = o;
                con.ifname = $(this).find('.address').attr('data-name');
                con.mac = $(this).find('.address').attr('data-mac');
                param.push(con);
            });

            paramPro.interfaces = param;
            paramPro = JSON.stringify(paramPro);
            //console.log(paramPro);
            $.ajax({
                url: "/ISAPI/System/Network/Interfaces",
                type: "PUT",
                data: paramPro,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        $('.save-successful').show();
                    } else {
                        //   $('.save-successful').hide();
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
    }
}();


//实时监控
var ssjk = function () {
    //将报警信息放入列表
    function showAlarm(alarmInfo) {
        alarmInfo = JSON.parse(alarmInfo);
        alarmLen++;
        if (alarmLen > 50) {
            //        alarmLen = 50; 
            //改变所有tr的id-1
            $('.list-table tbody tr').each(function () {
                var id = parseInt($(this).attr('id').split('_')[1]) - (alarmLen - 50);
                if (id == 0) {
                    $(this).remove();
                }
            });
        }

        if (alarmLen <= 10) {
            $('#totalPage').text(1);
        } else if (10 < alarmLen && alarmLen <= 20) {
            $('#totalPage').text(2);
        } else if (20 < alarmLen && alarmLen <= 30) {
            $('#totalPage').text(3);
        } else if (30 < alarmLen && alarmLen <= 40) {
            $('#totalPage').text(4);
        } else if (40 < alarmLen) {
            $('#totalPage').text(5);
        }

        var str = '<tr id="alarm_' + alarmLen + '"><td>' + alarmLen + '</td><td>' + alarmInfo.eventtype + '</td><td>' + alarmInfo.platename + '</td><td>' + alarmInfo.platecolor + '</td><td>' + alarmInfo.snaptime + '</td></tr>';
        if (alarmLen < 50) {
            $('#totalNum').text(alarmLen);
        } else {
            $('#totalNum').text(50);
        }

        $('.list-table tbody').prepend(str);

        if (parseInt($('#currentpage').text()) != 1) {
            $('#alarm_' + alarmLen).hide();
        } else if (alarmLen >= 10) { //只显示后添加的十条记录
            var k = alarmLen - 10;
            $('#alarm_' + k).hide(); //隐藏tr;  
        }

    };

    function showPage() {
        if ($('#yema').val() > 0) {
            var j = parseInt($('#yema').val());
            var totalPage = parseInt($('#totalPage').text());
            var totalNum = parseInt($('#totalNum').text());
            if (totalPage >= j) {
                $('#currentpage').html(j);
                if (j == 1) { //第一页
                    if (totalNum <= 10) {

                    } else if (10 < totalNum && 　totalNum <= 20) { //显示 11-20
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (id > (alarmLen - 10)　 && 　id <= alarmLen) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });

                    } else if (20 < totalNum && 　totalNum <= 30) { //显示 21-30
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (alarmLen - 10 < id && 　id <= alarmLen) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (30 < totalNum && 　totalNum <= 40) { //显示 31-40
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (alarmLen - 10 < id && 　id <= alarmLen) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (40 < totalNum) { //显示 41-50
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (alarmLen - 10 < id) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    }

                } else if (j == 2) {
                    if (totalNum <= 20) {
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (id <= totalNum - 10) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (20 < totalNum && 　totalNum <= 30) { //显示 20-11 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((totalNum - 20) < id && id <= (totalNum - 10)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (30 < totalNum && 　totalNum <= 40) { //显示 21-30 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((totalNum - 20) < id && 　id <= (totalNum - 10)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (40 < totalNum) { //显示 31-40 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((alarmLen - 20) < id && 　id <= (alarmLen - 10)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    }
                } else if (j == 3) {
                    if (totalNum <= 20) {

                    } else if (20 < totalNum && 　totalNum <= 30) { //显示 1-10
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (id <= (totalNum - 20)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (30 < totalNum && 　totalNum <= 40) { //显示 11-21 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((totalNum - 30) < id && id <= (totalNum - 20)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (40 < totalNum) { //显示 21-30 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((alarmLen - 30) < id && id <= (alarmLen - 20)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    }
                } else if (j == 4) {
                    if (totalNum <= 30) {} else if (30 < totalNum && 　totalNum <= 40) { //显示 1-10 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (id <= (totalNum - 30)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    } else if (40 < totalNum) { //显示 11-20 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if ((alarmLen - 40) < id && 　id <= (alarmLen - 30)) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    }
                } else if (j == 5) {
                    if (totalNum <= 40) {} else if (40 < totalNum) { //显示 1-10 
                        $('.list-table tbody tr').each(function () {
                            var id = parseInt($(this).attr('id').split('_')[1]);
                            if (id <= alarmLen - 40) {
                                $(this).show();
                            } else {
                                $(this).hide();
                            }
                        });
                    }
                }
            }
        }
    };

    function doPreview(url) {
        var plugin = document.getElementById("plugin1");
        if (plugin.Version != null && plugin.Version != undefined) {
            var pv = plugin.Version.substr(1);
            if (!plugin.valid || pv < pluginVer) return;
            if (bPluginInit) {
                preview_pluginLoad(url);
                var iRet = plugin.StartPlay();
                if (iRet < 0) {
                    alert("视频连接失败，请检查视频设置信息是否正确!"); //,errno:"+iRet);
                    return;
                }
                $('.ssjk-yulan').addClass('active');
            } else {
                plugin.StopPlay();
                $('.ssjk-yulan').removeClass('active');
            }
        }
    };


    function setManualSnap(message) {
        var proParam = {};
        var conParam = eval('(' + message + ')');

        var newData = {};
        var newData2 = {};

        newData.left = parseInt(conParam.left);
        newData.bottom = parseInt(conParam.bottom);
        newData.top = parseInt(conParam.top);
        newData.right = parseInt(conParam.right);

        newData2.snapeara = newData;
        newData2.eventid = 1;

        proParam.manualsnap = newData2;

        proParam = JSON.stringify(proParam);
        //  alert(proParam);
        $.ajax({
            url: "/ISAPI/Analysis/ManualSnap?chnid=1",
            type: "PUT",
            data: proParam,
            datatype: 'json',
            cache: false,
            async: false,
            headers: {
                "Authorization": "Basic " + localStorage.authorization
            },
            success: function (data) {
                var result = data.result;
                if (result.code == 0) { //获取数据成功

                } else {
                    alert('手动抓拍失败!');
                }
            },
            error: function () {
                child.logout();
            }
        });
    }

    function preview_pluginLoad(url) {
        var plugin = document.getElementById("plugin1");
        var serverip = document.location.hostname;
        if (plugin.valid) {
            //var videourl="rtsp://admin:jaya2015@192.168.1.223/0";
            plugin.SetFunction(0);
            if (plugin.Version != null && plugin.Version != undefined) {
                var pv = plugin.Version.substr(1);
                //  console.log(pv < pluginVer);
                if (pv < pluginVer) {
                    $(".player-pictures").show();
                    $("#showPlayer").hide();
                    bPluginInit = false;
                } else {
                    $("#showPlayer").show();
                    $(".player-pictures").hide();
                    bPluginInit = true;
                }
                if (url != '') {
                    plugin.SetRtspUrl(url);
                    addEvent(plugin, 'alarm', showAlarm);
                    addEvent(plugin, 'setManualAnalyseParams', setManualSnap);
                    plugin.SetServerIp(serverip);

                }
            }
        } else {
            $(".player-pictures").show();
            $("#showPlayer").hide();
            bPluginInit = false;
        }
    };

    function doFullScreen() {
        var plugin = document.getElementById("plugin1");
        plugin.OpenFullScreen();
    };

    //获取存储路径 cmd=0选取截取图片存放路径   cmd=1选取录像存储路径
    function snapPicture(cmd) {
        var plugin = document.getElementById("plugin1");
        var lj = plugin.GetSaveDirectory(cmd);
        if (lj == '' || lj == null) {
            alert('请先设置图片存储路径！');
            return;
        } else {
            var iRet = plugin.SnapPicture();
            if (iRet != 0) {
                alert("操作失败！");
            } else {
                alert("截取成功!");
            }
        }
    };

    function doRecord(cmd, flag) {
        var plugin = document.getElementById("plugin1");
        var path = plugin.GetSaveDirectory(cmd);
        if (path == "" || path == null) {
            alert("请先选择录像存储路径!");
            return;
        } else {
            if (flag) {
                if (plugin.valid) {
                    var iRet = plugin.StartRecord(path);
                    if (iRet < 0) {
                        alert("start record failed");
                        return;
                    }
                    $(".player-cz-vedio").hide();
                    $(".player-cz-vedio2").show();
                    localStorage.startRecord = 'true';
                } else {
                    alert("start record failed");
                }
            } else {
                plugin.StopRecord();
                $(".player-cz-vedio").show();
                $(".player-cz-vedio2").hide();
                localStorage.startRecord = 'false';
            }
        }
    };


    function doYunOperation(cases, events) {
        var param = {};
        var con = {};
        var type = '';
        if (events) {
            type = "PUT";
        } else {
            type = "DELETE";
        }
        con.action = cases;
        con.velocity = parseInt($("#ScrollBarvalue").val());
        con.timeout = 0;
        param.ptzmove = con;
        param = JSON.stringify(param);
        $.ajax({
            url: "/ISAPI/VideoIn/PTZ/Move?chnid=1",
            type: type,
            data: param,
            datatype: 'json',
            cache: false,
            async: false,
            headers: {
                "Authorization": "Basic " + localStorage.authorization
            },
            success: function (data) {
                var code = data.result.code;
                if (code == 0) {

                } else {
                    alert(data.result.description);
                    return;
                }
            },
            error: function () {
                child.logout();
            }
        });

    };

    function dozhuapai() {
        var plugin = document.getElementById("plugin1");
        plugin.DoManualAnalyse(0);
    }
    return {
        addEvent: function () {
            child.addEvent();
            ssjk.init();
            //开启预览，开启抓拍
            //            $(".ssjk-offon span").click(function () {
            //                $(this).toggleClass("active");
            //            });
            //播放器按钮操作

            $('.snapPicture').click(function () {
                snapPicture(0);
            });
            $('.startRecord').click(function () {
                doRecord(1, true);
            });
            $('.stopRecord').click(function () {
                doRecord(1, false);
            });

            $('.fullscreen').click(function () {
                doFullScreen();
            });

            $(".player-cz-vedio").click(function () {
                $(".player-cz-vedio").hide();
                $(".player-cz-vedio2").show();
            });

            $(".player-cz-vedio2-pause").click(function () {
                $(this).toggleClass("green");
            });

            $(".player-cz-vedio2-stop").click(function () {
                $(".player-cz-vedio").show();
                $(".player-cz-vedio2").hide;
            });

            $('#fenyeGo').click(function () {
                showPage();
            });

            $(".btn_invoke").click(function () {
                var id = $('#presetlist').val();
                if (!isNaN(parseInt(id))) {
                    if (ssjk.getSwitch()) {
                        alert("算法正在运行中，无法调用预置位!");
                        return;
                    } else {
                        ssjk.getPreset(id);
                    }
                }
            });
            $(".btn-rebuild").click(function () { //新建预置位
                //  var id = $('#presetlist').val();
                ssjk.postPreset();
            });

            $('.operateYuntai a').bind('mousedown', function () {
                var cases = parseInt($(this).attr("data-type"));
                doYunOperation(cases, true);
            }).mouseup(function () {
                var cases = parseInt($(this).attr("data-type"));
                doYunOperation(cases, false);
            });

            $("#ssjk_sub").click(function () {
                preview_pluginLoad(localStorage.videourlSub);
                doPreview(localStorage.videourlSub);
            });

            $("#ssjk_main").click(function () {
                preview_pluginLoad(localStorage.videourlMain);
                doPreview(localStorage.videourlMain);
            });
            $("#openPreview").click(function () { //开启预览
                $(this).toggleClass("active");
                var type = $("#ssjk_sub").prop("checked") ? localStorage.videourlSub : localStorage.videourlMain;
                if ($(this).hasClass('active')) {
                    bPluginInit = true;
                    doPreview(type);
                } else {
                    $("#doCaptureByHand").removeClass('active');

                    bPluginInit = false;
                    doPreview(type);
                }
            });
            $("#openCapture").click(function () {
                $(this).toggleClass("active");
                var flag = $(this).hasClass('active');
                if (flag) {} else {
                    $("#doCaptureByHand").removeClass('active');
                }
                ssjk.doCapture(flag);
            });

            //            $("#presetlist").change(function () {
            //              
            //            });
            $("#doCaptureByHand").click(function () {
                if ($('#openCapture').hasClass('active') && $('#openPreview').hasClass('active')) {
                    $(this).toggleClass("active");
                    if ($(this).hasClass('active')) {
                        dozhuapai();
                    }

                }
            });

            $(".btn_setting").click(function () {
                var id = $('#presetlist').val();
                if (!isNaN(parseInt(id))) {
                    if (ssjk.getSwitch()) {
                        alert("算法正在运行中，无法操作预置位!");
                        return;
                    } else {
                        ssjk.setPreset('PUT', parseInt(id));
                    }

                }
            });
            $(".btn_delete").click(function () {
                var id = $('#presetlist').val();
                if (!isNaN(parseInt(id))) {
                    if (ssjk.getSwitch()) {
                        alert("算法正在运行中，无法操作预置位!");
                        return;
                    } else {
                        ssjk.setPreset('DELETE', parseInt(id));
                        ssjk.getPresetList();
                    }
                }
            });
        },
        getPresetList: function () {
            $.ajax({
                url: "/ISAPI/VideoIn/PTZ/PresetList?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        var list = data.presetlist;
                        var str = "";
                        $("#presetlist").empty();
                        if (list && list.length > 0) {
                            for (var i = 0; i < list.length; i += 1) {
                                str += '<option value="' + list[i].presetid + '">' + list[i].presetid + '</option>';
                            }
                            $("#presetlist").append(str);
                            var id = $("#presetlist").val();
                            if (!isNaN(parseInt(id)) && !ssjk.getSwitch()) {
                                ssjk.getPreset(id);
                            }
                        }
                    } else {
                        //alert('获取预置位列表失败!');
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setPreset: function (type, num) {
            $.ajax({
                url: "/ISAPI/VideoIn/PTZ/Preset?chnid=1&presetid=" + num,
                type: type,
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功

                    } else {
                        alert('操作失败!');
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getPreset: function (num) {
            $.ajax({
                url: "/ISAPI/VideoIn/PTZ/Preset?chnid=1&presetid=" + num,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功

                    } else {
                        // alert('算法正在运行中,无法获取预置位!');
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        postPreset: function (num) {
            $.ajax({
                url: "/ISAPI/VideoIn/PTZ/Preset?chnid=1&presetid=" + num,
                type: "POST",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        ssjk.getPresetList();
                    } else {
                        alert('算法正在运行中,无法添加预置位!');
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getSwitch: function () {
            var openCapture = true;
            $.ajax({
                url: "/ISAPI/Analysis/Switch?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        if (data.switch) {
                            $("#openCapture").addClass('active');
                            openCapture = true;
                        } else {
                            $("#openCapture").removeClass('active');
                            openCapture = false;
                        }
                    } else {
                        alert('获取算法状态失败!');
                    }
                },
                error: function () {
                    child.logout();
                }
            });
            return openCapture;
        },
        doCapture: function (capture) {
            var param = {};
            param.switch = capture;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Switch?chnid=1",
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) {

                    } else {
                        $("#openCapture").toggleClass("active");
                        alert(data.code.description);
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        init: function () {
            alarmLen = 0;
            ssjk.getSwitch(); //算法是否打开
            ssjk.getVideoInStatus();
            $('#scrollBar').slider({
                range: "min",
                min: 0,
                max: 100,
                value: 50,
                slide: function (event, ui) {
                    $(this).next().val(ui.value);
                }
            });
            $('#ScrollBarvalue').val(50);
            ssjk.getPresetList();
        },
        getVideoInStatus: function () { //获取视频状态   
            $.ajax({
                url: "/ISAPI/VideoIn/Status?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) { //获取数据成功
                        var Stat = data.videoinStatus[0].streamStat;
                        if (Stat == 1) {
                            ssjk.getVideoInStream();
                        } else {
                            alert('视频源异常，请检查视频设置信息是否正确!');
                            localStorage.videourlMain = "";
                            localStorage.videourlSub = "";
                        }
                    } else {
                        alert('获取视频源状态失败!');
                        localStorage.videourlMain = "";
                        localStorage.videourlSub = "";
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getVideoInStream: function () { //获取视频流     
            $.ajax({
                url: "/ISAPI/VideoIn/Stream?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var result = data.result;
                    if (result.code == 0) {
                        var streamlist = data.streamlist;
                        localStorage.videourlMain = streamlist[0].main.rtsp;
                        localStorage.videourlSub = streamlist[0].sub.rtsp;
                    } else {
                        localStorage.videourlMain = "";
                        localStorage.videourlSub = "";
                    }
                    $("#ssjk_main").prop("checked", true);
                    $("#ssjk_sub").prop("checked", false);
                    preview_pluginLoad(localStorage.videourlMain);
                    doPreview(localStorage.videourlMain);
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }

}();
//录像回放
var lxhf = function () {

    return {
        formateDisktype: function (num) { // 0-ROM 1-SD 2-USB 3-HDD

            if (num === null || num === '' || num === undefined) return;
            num = parseInt(num);
            var str = '';
            switch (num) {
                case 0:
                    str = "ROM"
                    break;
                case 1:
                    str = "SD"
                    break;
                case 2:
                    str = "USB"
                    break;
                case 3:
                    str = "HDD"
                    break;
                default:
                    str = "其它"
                    break;
            }
            return str;
        },
        addEvent: function () {
            child.addEvent();

            var myDate = new Date();
            var date = myDate.toLocaleDateString().replace(/\//g, "-").concat();
            $("#currentday").val(date);

            $('.searchPage').click(function () {
                var num = parseInt($('.pageNum').val());
                var currentPage = parseInt($('#pageno').html());
                var totalPage = parseInt($('#pagecnt').html());
                if (isNaN(num) || num == currentPage || num > totalPage || num < 1 || num == '' || num == null) return;
                lxhf.init(2);
            });
            $(".firstPage").click(function () {
                var currentPage = parseInt($('#pageno').html());
                if (currentPage == 1) return;
                $('#pageno').html('1');
                lxhf.init(1);
            });

            $(".lastPage").click(function () {
                var currentPage = parseInt($('#pageno').html());
                var totalPage = parseInt($('#pagecnt').html());
                if (currentPage == totalPage) return;
                $('#pageno').html($('#pagecnt').html());
                lxhf.init(1);
            });

            $(".prevPage").click(function () {
                var currentPage = parseInt($('#pageno').html());
                var totalPage = parseInt($('#pagecnt').html());
                if (currentPage <= totalPage && currentPage > 1) {
                    $('#pageno').html(currentPage - 1);
                    lxhf.init(1);
                }
            });
            $(".nextPage").click(function () {
                var currentPage = parseInt($('#pageno').html());
                var totalPage = parseInt($('#pagecnt').html());
                if (currentPage < totalPage) {
                    $('#pageno').html(currentPage + 1);
                    lxhf.init(1);
                }
            });

            $('.btn-search').bind("click", function () {
                lxhf.init(3);
            });
            $("#contentVideo").on('click', '.playVideo', function () {
                $("#showVideo").hide();
                $(".player-pictures").hide();
                $(".loading-video").show();
                $("#video").attr('src', $(this).next().attr('href'));
            });

            $("#contentVideo").on('click', '.delRecordHui', function () {
                var did = parseInt($(this).attr('data-id'));
                if (confirm("确定要删除该条记录?")) {
                    $.ajax({
                        url: "/ISAPI/Record/Operate?rowid=" + did,
                        type: "DELETE",
                        cache: false,
                        async: false,
                        headers: {
                            "Authorization": "Basic " + localStorage.authorization
                        },
                        success: function (data) {
                            var code = data.result.code;
                            if (code == 0) { //删除成功
                                lxhf.init(2);
                            } else {
                                return;
                            }
                        }
                    });
                }

            });

            $("#contentVideo").on('click', '.copyRecord', function () {
                var types = parseInt($(this).attr('data-type'));
                var rowsid = parseInt($(this).next().attr('data-id'));
                if (types == 1) {
                    $("#SDCard").hide();
                    $("#UDisk").show();
                } else if (types == 2) {
                    $("#SDCard").show();
                    $("#UDisk").hide();
                } else {
                    $("#SDCard,#UDisk").show();
                }
                $("#saveID").val(rowsid);
                $("#sRowid").text("序号" + rowsid + " ");
                $("#saveasDialog").show();
            });


            $(".cancelDialog").bind("click", function () {
                $("#saveasDialog").hide();
            });

            $("#SDCard").click(function () {
                $("#saveasDialog").hide();
                var did = $("#saveID").val();
                $.ajax({
                    url: "/ISAPI/Record/Operate?rowid=" + did + "&disktype=1",
                    type: "PUT",
                    cache: false,
                    async: false,
                    headers: {
                        "Authorization": "Basic " + localStorage.authorization
                    },
                    success: function (data) {
                        var code = data.result.code;
                        if (code == 0) {
                        } else {
                            alert(data.result.description);
                            return;
                        }
                    }
                });
            });

            $("#UDisk").click(function () {
                $("#saveasDialog").hide();
                var did = $("#saveID").val();
                $.ajax({
                    url: "/ISAPI/Record/Operate?rowid=" + did + "&disktype=2",
                    type: "PUT",
                    cache: false,
                    async: false,
                    headers: {
                        "Authorization": "Basic " + localStorage.authorization
                    },
                    success: function (data) {
                        var code = data.result.code;
                        if (code == 0) {} else {
                            alert(data.result.description);
                            return;
                        }
                    }
                });
            });

            //             document.getElementById('video').addEventListener('onloadstart',function(){   
            //                   $(".loading-video").show();  
            //             }); 
            document.getElementById('video').addEventListener('canplay', function () {
                $(".loading-video").hide();
                $("#showVideo").show();
            });
        },
        init: function (num) {
            if (num == 3) {
                if (!$("#singleDayVideo").validationEngine("validate")) {
                    return;
                }
            }
            pageSize = 10;
            lxhf.getRecord(num);
        },
        getRecord: function (num) {
            var params = new Object();
            var con = new Object();
            con.start = $("#currentday").val() + ' ' + $('#starttime').val();
            con.end = $("#currentday").val() + ' ' + $('#endtime').val();
            con.type = 0; //常规
            con.chnid = 1;
            if (num == 1 || num == 3) {
                con.pageno = parseInt($('#pageno').html());
            } else {
                con.pageno = $('.pageNum').val() == null || $('.pageNum').val() == '' ? 1 : parseInt($('.pageNum').val());
            }
            con.pagesize = pageSize;
            params.recquery = con;
            var jsonAuth = JSON.stringify(params);

            $.ajax({
                url: "/ISAPI/Record/Query",
                type: "PUT",
                data: jsonAuth,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var re = new Array();
                        re = data.recresult;
                        // var pagecnt = re.pagecnt;
                        var pageno = re.pageno;
                        var reccnt = re.reccnt;
                        var list = re.filelist;

                        var str = '';
                        if (list != null) {
                            for (var i = 0; i < list.length; i++) {
                                //                                str += "<tr><td>" + list[i].rowid + '</td><td>' + list[i].start + '</td><td>' + list[i].end + '</td><td>常规</td>' +
                                //                                    '<td><a class="icon-cz icon-shanchu delRecord" data-id="' + list[i].rowid + '" title="删除"></a></td>' +
                                //                                    '<td><a class="icon-cz icon-yulan recordVideo" data-video=' + list[i].path + ' title="预览"></a></td></tr>';
                                var time = list[i].start.split(" ")[1] + "-" + list[i].end.split(" ")[1];
                                var path = list[i].path;
                                var name = list[i].path.split("/").pop();

                                str += '<tr><td>' + list[i].rowid + '</td><td>' + time + '</td>' +
                                    '<td>常规</td><td>CH1</td><td>' + name + '</td><td>' + list[i].size + '</td>' +
                                    '<td>' + lxhf.formateDisktype(list[i].disktype) + '</td>' +
                                    '<td><a class="icon-cz icon-bf playVideo" title="播放录像"></a><a class="icon-cz icon-xz" download="' + name + '" href="' + path + '" title="下载录像"></a>' +
                                    '<a class="icon-cz icon-fz copyRecord" data-type"' + list[i].disktype + '" title="复制录像"></a><a class="icon-cz icon-shanchu delRecordHui" data-id="' + list[i].rowid + '" title="删除录像"></a></td></tr>';
                            }
                        }
                        $('#contentVideo').empty().append(str);
                        var pagecnt = Math.ceil(reccnt / pageSize);
                        $('#pagecnt').text(pagecnt);
                        $('#pageno').text(pageno);
                        $('#reccnt').text(reccnt);

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }
}();

//分析规则
var analysisRules = function () {
    function onSaveArea(nAreaType, nIndex) {}

    function onDeleteArea(nAreaType, nIndex) {}

    function onClickArea(nAreaType, nIndex) {}

    function onStartDraw(cmd) {
        var plugin = document.getElementById("plugin0");
        if (cmd == 3 || cmd == 9) {
            var clbw = parseFloat($('#clbwidth').val());
            var clbl = parseFloat($('#clblen').val());
            plugin.SetTargetSize(clbw, clbl, -1, -1);
        }
        plugin.StartDraw(cmd);
    };

    function AlgSetting_pluginLoaded(videourl, eventParams) {
        var plugin = document.getElementById("plugin0");
        var serverip = document.location.hostname;
        if (plugin.valid) {
            //var url="rtsp://admin:jaya2015@192.168.1.223/0";
            plugin.SetFunction(2);
            plugin.SetRtspUrl(videourl);

            plugin.SetServerIp(serverip);
            addEvent(plugin, 'saveArea', onSaveArea);
            addEvent(plugin, 'deleteArea', onDeleteArea);
            addEvent(plugin, 'clickArea', onClickArea);
            plugin.StartPlay();
            plugin.SetAnalyseParams(JSON.stringify(eventParams));
        }
    };

    function doOperation(cases, events) {
        var param = {};
        var con = {};
        var type = '';
        if (events) {
            type = "PUT";
        } else {
            type = "DELETE";
        }
        con.action = cases;
        con.velocity = parseInt($("#yutaiSpeed").next().val());
        con.timeout = 0;
        param.ptzmove = con;
        param = JSON.stringify(param);
        $.ajax({
            url: "/ISAPI/VideoIn/PTZ/Move?chnid=1",
            type: type,
            data: param,
            datatype: 'json',
            cache: false,
            async: false,
            headers: {
                "Authorization": "Basic " + localStorage.authorization
            },
            success: function (data) {
                var code = data.result.code;
                if (code == 0) {

                } else {
                    alert(data.result.description);
                    return;
                }
            },
            error: function () {
                child.logout();
            }
        });

    };
    return {
        addEvent: function () {
            child.addEvent();
            eventParams = {};
            eventParams.code = -1;
            analysisRules.init();

            $("#fxgzValidate").validationEngine("attach", {
                promptPosition: "centerRight",
                scroll: false
            });

            $('#yutaiSpeed').slider({
                range: "min",
                min: 0,
                max: 100,
                value: 80,
                slide: function (event, ui) {
                    $(this).next().val(ui.value);
                }
            });
            $('#yutaiSpeed').next().val(80);

            $('.operateYuntai a').bind('mousedown', function () {
                var cases = parseInt($(this).attr("data-type"));
                doOperation(cases, true);
            }).mouseup(function () {
                var cases = parseInt($(this).attr("data-type"));
                doOperation(cases, false);
            });

            $("#targetSize").click(function () {
                analysisRules.onCheckboxClick();
            });

            $(".save-btn").click(function () {
                if (!$("#fxgzValidate").validationEngine("validate")) {
                    return;
                }
                var sid = parseInt($("#sceneSelect").val());
                if (analysisRules.getAnalysisSwitch()) {
                    if (confirm("算法正在运行中，是否关闭?")) {
                        analysisRules.setAnalysisSwitch();

                        analysisRules.setScene(sid);
                        analysisRules.setScenRule(sid);
                    } else {
                        analysisRules.setScene(sid);
                        analysisRules.setScenRule(sid);
                    }
                } else {
                    analysisRules.setScene(sid);
                    analysisRules.setScenRule(sid);
                }
            });

            $("#sceneSelect").change(function () {
                if (analysisRules.getAnalysisSwitch()) {
                    if (confirm("算法正在运行中，是否关闭?")) {
                        analysisRules.setAnalysisSwitch();
                        analysisRules.getVideoInStatus();
                    } else {
                        analysisRules.getVideoInStatus();
                    }
                } else {
                    analysisRules.getVideoInStatus();
                }
            });
        },
        init: function () {
            //   analysisRules.setAnalysisSwitch();

            if (analysisRules.getAnalysisSwitch()) {
                if (confirm("算法正在运行中，是否关闭?")) {
                    analysisRules.setAnalysisSwitch();
                    analysisRules.getVideoSid();
                } else {
                    analysisRules.getVideoSid();
                }
            } else {
                analysisRules.getVideoSid();
            }

        },
        getAnalysisSwitch: function () { //算法状态
            var flag = false;
            $.ajax({
                url: "/ISAPI/Analysis/Switch?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        flag = data.switch;
                    } else {
                        alert("视频源异常!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
            return flag;
        },
        setAnalysisSwitch: function () { //算法状态 
            var param = {};
            param.switch = false;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Switch?chnid=1",
                datatype: 'json',
                data: param,
                type: "PUT",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {} else {
                        alert("视频源异常!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getVideoSid: function () { //获取所有场景编号,支持的能力
            $.ajax({
                url: "/ISAPI/Analysis/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var maxSceneNum = data.analysisCap.maxSceneNum;
                        var str = '';
                        for (var i = 0; i < maxSceneNum; i += 1) {
                            str += '<option value="' + (i + 1) + '">场景' + (i + 1) + '</option>';
                        }
                        $("#sceneSelect").empty().append(str);

                        var obj1 = $("#supportCalibrate");
                        var obj2 = $("#supportAnalyArea");
                        var obj3 = $("#supportShieldArea");
                        var obj4 = $("#supportPolyline");

                        if (data.analysisCap.supportCalibrate) {
                            obj1.removeClass('btn-gray');
                            obj1.bind("click", function () {
                                onStartDraw(obj1.attr("data-type"));
                            });
                        } else {
                            obj1.addClass('btn-gray');
                            obj1.unbind("click");
                            $("#hideCalibrate").hide();
                        }
                        if (data.analysisCap.supportAnalyArea) {
                            obj2.removeClass('btn-gray');
                            obj2.bind("click", function () {
                                onStartDraw(obj2.attr("data-type"));
                            });
                        } else {
                            obj2.addClass('btn-gray');
                            obj2.unbind("click");
                        }
                        if (data.analysisCap.supportShieldArea) {
                            obj3.removeClass('btn-gray');
                            obj3.bind("click", function () {
                                onStartDraw(obj3.attr("data-type"));
                            });
                        } else {
                            obj3.addClass('btn-gray');
                            obj3.unbind("click");
                        }

                        if (data.analysisCap.supportPolyline) {
                            obj4.removeClass('btn-gray');
                            obj4.bind("click", function () {
                                onStartDraw(obj4.attr("data-type"));
                            });
                        } else {
                            obj4.addClass('btn-gray');
                            obj4.unbind("click");

                        }

                        analysisRules.getVideoInStatus();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getVideoInStatus: function () { //视频源状态
            $.ajax({
                url: "/ISAPI/VideoIn/Status?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var streamStat = data.videoinStatus[0].streamStat;
                        if (streamStat == 1) { //视频源正常 
                            var sceneid = $("#sceneSelect").val();
                            if (sceneid == null || sceneid == '') {} else {
                                analysisRules.getSource(sceneid);
                            }
                        }
                    } else {
                        alert("视频源异常!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getSource: function (sid) { //视频资源
            $.ajax({
                url: "/ISAPI/VideoIn/Source?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var type = data.sourcelist[0].videosource.streamtype; //0-主码流 1-子码流
                        analysisRules.getStream(type, sid);
                    } else {
                        alert("视频源未配置!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getStream: function (type, sceneid) {
            $.ajax({
                url: "/ISAPI/VideoIn/Stream?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        //  var stream = data.
                        var stream = type == 0 ? data.streamlist[0].main.rtsp : data.streamlist[0].sub.rtsp;
                        analysisRules.getScene(sceneid);
                        analysisRules.getScenRule(stream, sceneid);
                    } else {
                        alert("获取信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getScene: function (num) {
            $.ajax({
                url: "/ISAPI/Analysis/Scene?chnid=1&sceneid=" + num,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $("#presetid").val(data.scene.presetid);
                        $("#waittime").val(data.scene.waittime);
                        $("#scenename").val(data.scene.scenename);
                        $("#sceneDirect").val(data.scene.direction);
                        if (data.scene.presetid > 0 && !analysisRules.getAnalysisSwitch()) {
                            analysisRules.getPreset(num);
                        }
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setScene: function (num) {
            var param = {};
            var con = {};
            con.presetid = parseInt($("#presetid").val());
            con.scenename = $("#scenename").val();
            con.waittime = parseInt($("#waittime").val());
            con.direction = parseInt($("#sceneDirect").val());
            param.scene = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Scene?chnid=1&sceneid=" + num,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        onCheckboxClick: function () {
            //  console.log($('#targetSize').is(':checked'));
            var plugin = document.getElementById("plugin0");
            if ($('#targetSize').is(':checked')) {
                plugin.SetAreaHideFlag(7, 0);
            } else {
                plugin.SetAreaHideFlag(7, 1);
            }
        },
        getScenRule: function (stream, num) {
            $.ajax({
                url: "/ISAPI/Analysis/Scene/Rule?chnid=1&sceneid=" + num,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                        $("#clblen").val(data.rule.clblen);
                        $("#clbwidth").val(data.rule.clbwidth);
                        $("#targetSize").prop("checked", data.rule.enableise);
                        var eventInfo = {};
                        eventInfo.lanecnt = 0;
                        eventInfo.lane = null;
                        eventInfo.uturnline = null;
                        eventInfo.uturnlinecnt = 0;
                        eventInfo.changelinecnt = 0;
                        eventInfo.changeline = null;

                        eventInfo.sheildarea = data.rule.sheildarea;
                        eventInfo.sheildareacnt = data.rule.sheildareacnt;
                        eventInfo.solidline = data.rule.solidline;
                        eventInfo.solidlinecnt = data.rule.solidlinecnt;
                        eventInfo.stoparea = data.rule.stoparea;
                        eventInfo.stopareacnt = data.rule.stopareacnt;
                        eventInfo.minsize = data.rule.minsize;
                        eventInfo.maxsize = data.rule.maxsize;

                        eventParams.clbcoord = data.rule.clbcoord;
                        eventParams.eventInfo = eventInfo;
                        eventParams.code = 0;
                        AlgSetting_pluginLoaded(stream, eventParams);
                        var plugins = document.getElementById("plugin0"); 
                        plugins.SetAreaHideFlag(7, data.rule.enableise ? 0 : 1); 
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setScenRule: function (num) {
            var param = {};
            var con = {};
            var savePluginParamStr = document.getElementById("plugin0").SaveAnalyseParams();
            var savePluginParam = eval('(' + savePluginParamStr + ')');
            con.clbcoord = savePluginParam.clbcoord;
            var ev = savePluginParam.eventInfo;
            con.sheildareacnt = ev.sheildareacnt;
            con.sheildarea = ev.sheildarea;
            con.stopareacnt = ev.stopareacnt;
            con.stoparea = ev.stoparea;
            con.minsize = ev.minsize;
            con.maxsize = ev.maxsize;
            con.solidlinecnt = ev.solidlinecnt;
            con.solidline = ev.solidline;
            con.clblen = parseInt($("#clblen").val());
            con.clbwidth = parseInt($("#clbwidth").val());
            param.rule = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Scene/Rule?chnid=1&sceneid=" + num,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getPreset: function (num) {
            $.ajax({
                url: "/ISAPI/VideoIn/PTZ/Preset?chnid=1&presetid=" + num,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {} else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
    }
}();
var windowobj;
//取证数据配置
var qzsjSetting = function () {
    function getDetermine(num) {
        num = parseInt(num);
        var dev = {};

        $.ajax({
            url: "/ISAPI/Analysis/Evidence/Determine?eventid=" + num,
            type: "GET",
            cache: false,
            async: false,
            headers: {
                "Authorization": "Basic " + localStorage.authorization
            },
            success: function (data) {
                var code = data.result.code;
                if (code == 0) {
                    var dav = data.determine;
                    windowobj = {};
                    for (var key in dav) {
                        windowobj[key] = dav[key];
                    }
                    dev = dav;
                } else {
                    alert(data.result.description);
                    return;
                }
            },
            error: function () {
                child.logout();
            }
        });
        return dev;
    };

    function getDetermineKey(value) {
        var key = '';
        if (value == 0) {
            key = 'value1';
        } else if (value == 1) {
            key = 'value2';
        } else if (value == 2) {
            key = 'fvalue1';
        } else {
            key = 'fvalue2';
        }
        return key;
    }

    function setsnaptype(type) {
        var str = "";
        if (type == 0) {
            str = '<option value="0" selected>远景</option><option value="1">近景</option><option value="2">特写</option>';
        } else if (type == 1) {
            str = '<option value="0" >远景</option><option value="1" selected>近景</option><option value="2">特写</option>';
        } else {
            str = '<option value="0" >远景</option><option value="1">近景</option><option value="2" selected>特写</option>';
        }
        return str;

    };

    function sliderTextChange() {
        $('#sizelimit_text').unbind('change');
        $('#sizelimit_text').bind('change', function () {
            var value = parseInt($(this).val());
            var id = $(this).prev().attr('id');
            if (value < 0 || value > 10240) {
                alert('不合法的数据');
                $(this).val(0);
                $('#' + id).slider({
                    range: "min",
                    min: 0,
                    max: 10240,
                    value: 0,
                    slide: function (event, ui) {
                        $(this).next().val(ui.value);
                    }
                });
                return;
            }

            $('#' + id).slider({
                range: "min",
                min: 0,
                max: 10240,
                value: value,
                slide: function (event, ui) {
                    $(this).next().val(ui.value);
                }
            });
        });

        $('#pretime_text,#afttime_text').unbind('change');
        $('#pretime_text,#afttime_text').bind('change', function () {
            var value = parseInt($(this).val());
            var id = $(this).parent().prev().find("div").attr('id');
            if (value < 0 || value > 60) {
                alert('不合法的数据');
                $(this).val(0);
                $('#' + id).slider({
                    range: "min",
                    min: 0,
                    max: 60,
                    value: 0,
                    slide: function (event, ui) {
                        $("#" + id + "_text").val(ui.value);
                    }
                });
                return;
            }
            var text = "#" + id + "_text";

            $('#' + id).slider({
                range: "min",
                min: 0,
                max: 60,
                value: value,
                slide: function (event, ui) {
                    $(text).val(ui.value);
                }
            });
        });

    }
    return {
        addEvent: function () {
            child.addEvent();
            qzsjSetting.init();
            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });
            $("#qzsj_form").validationEngine("attach", {
                promptPosition: "centerRight",
                scroll: false
            });
            $('#fontcolor').colpick({
                layout: 'hex',
                submit: 0,
                colorScheme: 'dark',
                onChange: function (hsb, hex, rgb, el, bySetColor) {
                    $(el).css('border-color', '#' + hex);
                    // Fill the text box just if the color was set using the picker, and not the colpickSetColor function.
                    if (!bySetColor) $(el).val(hex);
                }
            }).keyup(function () {
                $(this).colpickSetColor(this.value);
            });

            $('#backcolor').colpick({
                layout: 'hex',
                submit: 0,
                colorScheme: 'dark',
                onChange: function (hsb, hex, rgb, el, bySetColor) {
                    $(el).css('border-color', '#' + hex);
                    // Fill the text box just if the color was set using the picker, and not the colpickSetColor function.
                    if (!bySetColor) $(el).val(hex);
                }
            }).keyup(function () {
                $(this).colpickSetColor(this.value);
            });

            $("#qzsj_eventtype").change(function () {
                qzsjSetting.initExpectEvent(0);
            });

            $("#picNums").change(function () {
                var value = $(this).val();
                var str = "";
                for (var i = 0; i < value; i += 1) {
                    str += '<tr><td>' + (i + 1) + '</td><td><select class="select-default" style="width: 100px;">' + setsnaptype(0) +
                        '</select></td><td><input type="text" value="0" class="input-default input-w100"/></td></tr>';
                }
                $("#picDetails").empty().append(str);
            });
            $("#picNums2").change(function () {
                var value = $(this).val();
                var str = "";
                for (var i = 0; i < value; i += 1) {
                    str += '<tr><td>' + (i + 1) + '</td><td><select class="select-default" style="width: 100px;">' + setsnaptype(0) +
                        '</select></td><td><input type="text" value="0" class="input-default input-w100"/></td></tr>';
                }
                $("#picDetails2").empty().append(str);
            });

            $(".save-btn").click(function () {
                if (!$("#qzsj_form").validationEngine("validate")) {
                    return;
                }
                var id = parseInt($("#qzsj_eventtype").val());
                $(".save-successful").hide();
                qzsjSetting.saveDetermine(id);
                qzsjSetting.saveModel(id);

                var mid = $(".eventNums").attr('data-mid');
                qzsjSetting.saveMakeup(parseInt(id), parseInt(mid));
                var mid2 = $(".eventNums2").attr('data-mid');
                qzsjSetting.saveMakeup(parseInt(id), parseInt(mid2));

                qzsjSetting.saveImgCompose(id);
                qzsjSetting.saveImgOSD(id);
                qzsjSetting.saveImgQuality(id);
                qzsjSetting.saveRecord(id);
                qzsjSetting.saveEvidenceCode(id);
                qzsjSetting.saveFileNaming(id);
            });

            sliderTextChange();
        },
        init: function () {
            $.ajax({
                url: "/ISAPI/Analysis/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.analysisCap;
                        var eventNum = device.maxAnalysisEvent;
                        var eventList = device.eventList;

                        var str = "";
                        for (var i = 0; i < eventNum; i++) {
                            str += "<option value='" + eventList[i].eventid + "'>" + eventList[i].name + "</option>";
                        };
                        $("#qzsj_eventtype").append(str);
                        $("#qzsj_eventtype").val(eventList[0].eventid); //初始化时选中第一个事件 

                        qzsjSetting.getCapability(eventList[0].eventid);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
            qzsjSetting.initExpectEvent(false);
        },
        initExpectEvent: function (flag) {
            var id = parseInt($("#qzsj_eventtype").val());
            if (flag) {} else {
                qzsjSetting.getCapability(id);
            };
            qzsjSetting.initImgOSD(id);
            qzsjSetting.initImgCompose(id);
            qzsjSetting.initImgQuality(id);
            qzsjSetting.initRecord(id);
            qzsjSetting.initEvidenceCode(id);
            qzsjSetting.initFileNaming(id);

        },
        getCapability: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.analysisCap;
                        var eventNum = device.maxAnalysisEvent;
                        var eventList = device.eventList;
                        var str = "";
                        var determine = getDetermine(id);
                        for (var i = 0; i < eventList.length; i++) {
                            if (eventList[i].eventid == id) {
                                var threshold = device.eventList[i].threshold; //初始化判决条件。
                                var evidenceModel = device.eventList[i].evidenceModel; //初始化抓拍模式。
                                var str2 = "";
                                var str3 = "";
                                for (var k = 0; k < threshold.length; k++) {
                                    var key = getDetermineKey(threshold[k].which);
                                    str2 += '<div class="quzheng-c-qiyong wtpd_condition" data-key="' + key + '">' + threshold[k].name + '：<input class="input-default input-w60 validate[required,custom[integer],min[' + threshold[k].valuescope.min + '],max[' + threshold[k].valuescope.max + ']]" type="text" value="' + determine[key] + '"/>' + threshold[k].unit + '</div>';

                                }

                                $(".addquzhengEvent").empty().append(str2);

                                for (var j = 0; j < evidenceModel.length; j++) {
                                    str3 += "<option value=" + evidenceModel[j].modelid + ">" + evidenceModel[j].name + "</option>";
                                }
                                $("#qzsj_zpms").empty().append(str3);
                                qzsjSetting.initModel(parseInt(id), evidenceModel);
                            }
                        }

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });

        },
        saveDetermine: function (id) {
            var param = {};
            var con = {};
            var list = windowobj;
            con = list;

            $(".wtpd_condition").each(function () {
                var key = $(this).attr("data-key");
                var value = parseInt($(this).find("input").val());
                con[key] = value;
            });

            param.determine = con;
            param = JSON.stringify(param);

            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Determine?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initModel: function (id, obj) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Model?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.evidencemodel;
                        var enableevent = dev.enableevent;
                        if (enableevent) {
                            $("#qysj_offon").addClass("a-offon-click");
                        } else {
                            $("#qysj_offon").removeClass("a-offon-click");
                        }
                        $("#qzsj_zpms").val(dev.model);
                        for (var j = 0; j < obj.length; j++) {
                            qzsjSetting.initImgMakeup(parseInt(id), parseInt(obj[j].modelid), obj[j].name); //0-二次抓拍 1-即停即走
                            //str3 += "<option value=" + evidenceModel[j].modelid + ">" + evidenceModel[j].name + "</option>";
                        }

                        //   qzsjSetting.initImgMakeup(parseInt(id), id2);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveModel: function (id) {
            var param = {};
            var con = {};
            if ($("#qysj_offon").hasClass("a-offon-click")) {
                con.enableevent = true;
            } else {
                con.enableevent = false;
            }

            con.model = parseInt($("#qzsj_zpms").val());
            param.evidencemodel = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Model?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        //    qzsjSetting.saveMakeup(parseInt(id), parseInt(con.model));
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initImgMakeup: function (id, modelid, name) {
            //    modelid = parseInt(modelid);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgMakeup?eventid=" + id + "&modelid=" + modelid,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.makeup;
                        var sequence = dev.sequence;
                        if (name == '二次抓拍') {
                            $(".eventNums").text(name);
                            $(".eventNums").attr('data-mid', modelid);
                            $("#picNums").val(dev.picNum);
                            var str = "";
                            for (var i = 0; i < sequence.length; i++) {
                                str += '<tr><td>' + (i + 1) + '</td><td><select class="select-default" style="width: 100px;">' + setsnaptype(sequence[i].snaptype) + '</select></td>' +
                                    '<td><input type="text" value="' + sequence[i].waittime + '" class="input-default input-w100" /> </td></tr>';
                            }
                            $("#picDetails").empty().append(str);
                        } else {
                            $(".eventNums2").text(name)
                            $(".eventNums2").attr('data-mid', modelid);
                            $("#picNums2").val(dev.picNum);
                            var str = "";
                            for (var i = 0; i < sequence.length; i++) {
                                str += '<tr><td>' + (i + 1) + '</td><td><select class="select-default" style="width: 100px;">' + setsnaptype(sequence[i].snaptype) + '</select></td>' +
                                    '<td><input type="text" value="' + sequence[i].waittime + '" class="input-default input-w100" /> </td></tr>';
                            }
                            $("#picDetails2").empty().append(str);
                        }

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },

        saveMakeup: function (id, modelid) {
            var param = {};
            var con = {};
            var arr = [];
            var mid = $(".eventNums").attr('data-mid');
            var mid2 = $(".eventNums2").attr('data-mid');
            if (modelid == mid) {
                con.picNum = parseInt($("#picNums").val());
                $("#picDetails").find("tr").each(function () {
                    var obj = {};
                    obj.snaptype = parseInt($(this).find(".select-default").val());
                    obj.waittime = parseInt($(this).find(".input-default").val());
                    arr.push(obj);
                });
            } else {
                con.picNum = parseInt($("#picNums2").val());
                $("#picDetails2").find("tr").each(function () {
                    var obj = {};
                    obj.snaptype = parseInt($(this).find(".select-default").val());
                    obj.waittime = parseInt($(this).find(".input-default").val());
                    arr.push(obj);
                });
            }

            con.sequence = arr;
            param.makeup = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgMakeup?eventid=" + id + "&modelid=" + modelid,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initImgOSD: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgOSD?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.osd;
                        var content = dev.content;
                        var fontcolor = dev.fontcolor;
                        var backcolor = dev.backcolor;
                        $("#posmodel").val(dev.posmodel);
                        for (var i in content) {
                            $("#" + i).prop("checked", content[i]);
                        }
                        $("#qzsj_fontsize").val(dev.fontsize);

                        $('#fontcolor').css('border-color', fontcolor);
                        $('#backcolor').css('border-color', backcolor);

                        $('#fontcolor').val(fontcolor.substring(1, fontcolor.length));
                        $('#backcolor').val(backcolor.substring(1, backcolor.length));

                        $("#qzsj_top").val(dev.top);
                        $("#qzsj_left").val(dev.left);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveImgOSD: function (id) {
            var param = {};
            var con = {};
            con.picNum = parseInt($("#picNums").val());
            var obj = {};
            $(".osd-list li input").each(function () {
                var id = $(this).attr('id');
                var value = $(this).prop("checked");
                obj[id] = value;
            });
            con.content = obj;
            con.posmodel = parseInt($("#posmodel").val());
            con.left = parseInt($("#qzsj_left").val());
            con.top = parseInt($("#qzsj_top").val());
            con.fontsize = parseInt($("#qzsj_fontsize").val());
            con.backcolor = "#" + $('#backcolor').val();
            con.fontcolor = "#" + $('#fontcolor').val();
            param.osd = con;
            param = JSON.stringify(param);

            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgOSD?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {} else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initImgCompose: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgCompose?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.compose;
                        $("#qzsj_com2").val(dev.com2);
                        $("#qzsj_com3").val(dev.com3);
                        $("#qzsj_com4").val(dev.com4);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveImgCompose: function (id) {
            var param = {};
            var con = {};
            con.com2 = parseInt($('#qzsj_com2').val());
            con.com3 = parseInt($('#qzsj_com3').val());
            con.com4 = parseInt($('#qzsj_com4').val());
            param.compose = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgCompose?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {} else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initImgQuality: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgQuality?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.quality;
                        var sizelimit = dev.sizelimit;
                        $("#sizelimit").val(sizelimit);
                        $('#sizelimit').slider({
                            range: "min",
                            min: 0,
                            max: 10240,
                            value: sizelimit,
                            slide: function (event, ui) {
                                $(this).next().val(ui.value);
                            }
                        });
                        $('#sizelimit').next().val(sizelimit);
                        $('#widthlimit').val(dev.widthlimit);
                        $('#heightlimit').val(dev.heightlimit);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveImgQuality: function (id) {
            var param = {};
            var con = {};
            con.heightlimit = parseInt($('#heightlimit').val());
            con.widthlimit = parseInt($('#widthlimit').val());
            con.sizelimit = parseInt($('#sizelimit_text').val());
            param.quality = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/ImgQuality?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initRecord: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Record?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.analyrecord;
                        var enable = dev.enable;
                        var pretime = dev.pretime;
                        var afttime = dev.afttime;
                        if (enable) {
                            $("#qzlx_offon").addClass("a-offon-click");
                        } else {
                            $("#qzlx_offon").removeClass("a-offon-click");
                        }
                        $('#pretime').slider({
                            range: "min",
                            min: 0,
                            max: 60,
                            value: pretime,
                            slide: function (event, ui) {
                                $("#pretime_text").val(ui.value);
                            }
                        });
                        $("#pretime_text").val(pretime);

                        $('#afttime').slider({
                            range: "min",
                            min: 0,
                            max: 60,
                            value: afttime,
                            slide: function (event, ui) {
                                $("#afttime_text").val(ui.value);
                            }
                        });
                        $("#afttime_text").val(afttime);
                        $("#record_model").val(dev.model);
                        $("#fileformat").val(dev.fileformat);
                        $("#mintime_text").val(dev.mintime);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveRecord: function (id) {
            var param = {};
            var con = {};
            if ($("#qzlx_offon").hasClass("a-offon-click")) {
                con.enable = true;
            } else {
                con.enable = false;
            }

            con.model = parseInt($("#record_model").val());
            con.fileformat = parseInt($("#fileformat").val());
            con.pretime = parseInt($('#pretime_text').val());
            con.afttime = parseInt($('#afttime_text').val());
            con.mintime = parseInt($("#mintime_text").val());
            param.analyrecord = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Record?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initEvidenceCode: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Code?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.eventcodelist;
                        var str = "";

                        str += '<div class="quzheng-c-qiyong">代码：<input type="text" class="input-default devcode" style="width:300px;" value="' + dev[0].eventcode.code + '" /></div>';
                        str += '<div class="quzheng-c-qiyong">描述：<input type="text" class="input-default devdesc" style="width:300px;" value="' + dev[0].eventcode.desc + '" /></div>';

                        $("#wfdm_qzsj").empty().append(str);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveEvidenceCode: function (id) {
            var con = {};
            var obj = {};
            obj.code = $(".devcode").val();
            obj.desc = $(".devdesc").val();
            con.eventcode = obj;
            con = JSON.stringify(con);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/Code?eventid=" + id,
                type: "PUT",
                data: con,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        initFileNaming: function (id) {
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/FileNaming?eventid=" + id,
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var dev = data.filenaming;
                        $("#timemodel").val(dev.timemodel);
                        $("#naming_img").val(dev.img);
                        $("#naming_rec").val(dev.rec);
                        $("#naming_eventimg").val(dev.eventimg);
                        $("#naming_eventrec").val(dev.eventrec);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveFileNaming: function (id) {
            var param = {};
            var con = {};
            con.eventrec = $("#naming_eventrec").val();
            con.eventimg = $("#naming_eventimg").val();
            con.timemodel = parseInt($("#timemodel").val());
            con.rec = $("#naming_rec").val();
            con.img = $("#naming_img").val();
            param.filenaming = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Evidence/FileNaming?eventid=" + id,
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
    }
}();

//抓拍计划
var zpjh = function () {
    function calcularTime(time) {
        var str;
        if (time <= 60) {
            if (time < 10) {
                time = "0" + time
            };
            str = "00:00:" + time;
        } else if (time > 60 && time < 3600) {
            var m = Math.floor(time / 60);
            var s = time - Math.floor(time / 60) * 60;
            if (m < 10) {
                m = "0" + m;
            }
            if (s < 10) {
                s = "0" + s;
            }
            str = "00:" + m + ":" + s;
        } else {
            var h = Math.floor(time / 3600);
            var m = Math.floor((time - h * 3600) / 60);
            var s = time - h * 3600 - m * 60;
            if (h < 10) {
                h = "0" + h;
            }
            if (m < 10) {
                m = "0" + m;
            }
            if (s < 10) {
                s = "0" + s;
            }
            str = h + ":" + m + ":" + s;
        }

        return str;
    };

    function parseToSecond(s) {
        var sp = s.split(":");
        return parseInt(sp[0]) * 3600 + parseInt(sp[1]) * 60 + parseInt(sp[2]);
    };

    function calcularDataStr(time) {
        var str = "";
        str += '<td><input style="width: 140px;height: 28px;margin:0 auto;"class="Wdate input-default" type="text" onfocus="WdatePicker({skin:\'whyGreen\',dateFmt:\'HH:mm:ss\', readOnly:false,onpicked:function(){}})" value=' + calcularTime(time) + ' readonly/></td>';
        return str;
    };
    return {
        addEvent: function () {
            child.addEvent();
            zpjh.init();
            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });

            $('.save-btn').click(function () {
                zpjh.setPlanSwitch();
            });

            $("#zpjs-quanbu").click(function () {
                $(".weekbox").prop("checked", true);
                $("#zpjs-fanxiang").prop("checked", false);
            });

            $("#zpjs-fanxiang").click(function () {
                $("#zpjs-quanbu").prop("checked", false);
                $('.weekbox').each(function () {
                    if ($(this).prop("checked")) {
                        $(this).prop("checked", false);
                    } else {
                        $(this).prop("checked", true);
                    }
                });
            });

            $(".tab_content tbody").on('click', '.cleardata', function () {
                $(this).parent().prev().find("input").val("00:00:00");
                $(this).parent().prev().prev().find("input").val("00:00:00");
            });

            $(".copyToWeek").click(function () {
                var week = $(".jh-tab").find(".active").attr("aweek"); //copy样本
                var clone = '';
                $(".weekbox").each(function () {
                    if ($(this).prop("checked")) {
                        var cloneTo = $(this).attr("data-aweek");
                        if (week == cloneTo) { //复制到的星期和当前星期一致时不需要复制

                        } else {
                            clone = $('#p-tabs-week' + week).contents().clone(true);
                            $('#p-tabs-week' + cloneTo).empty().append(clone);
                            clone = '';
                        }
                    }
                });
            });
        },
        init: function () {
            zpjh.getPlanSwitch();
            zpjh.getPlanCapabilities();
        },
        getPlanCapabilities: function () {
            $.ajax({
                url: "/ISAPI/Analysis/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.analysisCap;
                        zpjh.getPlan(device.maxPlanInDay);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getPlanSwitch: function () {
            $.ajax({
                url: "/ISAPI/Analysis/PlanSwitch?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.switch;
                        if (device) {
                            $(".a-offon").addClass("a-offon-click");
                        } else {
                            $(".a-offon").removeClass("a-offon-click");
                        }
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setPlanSwitch: function () {
            var switchs = false;
            var param = {};
            if ($(".a-offon").hasClass("a-offon-click")) {
                param = {
                    'switch': true
                };
            } else {
                param = {
                    'switch': false
                };
            }
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/PlanSwitch?chnid=1",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        zpjh.setPlan();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getPlan: function (maxplan) {
            $.ajax({
                url: "/ISAPI/Analysis/Plan?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.plan;
                        var mon_str, tue_str, wed_str, thu_str, fri_str, sat_str, sun_str;
                        var mon = device.mon;
                        var tue = device.tue;
                        var wed = device.wed;
                        var thu = device.thu;
                        var fri = device.fri;
                        var sat = device.sat;
                        var sun = device.sun;
                        for (var i = 0; i < maxplan; i++) {
                            mon_str += "<tr>";
                            mon_str += "<td>" + (i + 1) + "</td>";
                            mon_str += calcularDataStr(mon[i].s);
                            mon_str += calcularDataStr(mon[i].e);
                            mon_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            tue_str += "<tr>";
                            tue_str += "<td>" + (i + 1) + "</td>";
                            tue_str += calcularDataStr(tue[i].s);
                            tue_str += calcularDataStr(tue[i].e);
                            tue_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            wed_str += "<tr>";
                            wed_str += "<td>" + (i + 1) + "</td>";
                            wed_str += calcularDataStr(wed[i].s);
                            wed_str += calcularDataStr(wed[i].e);
                            wed_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            thu_str += "<tr>";
                            thu_str += "<td>" + (i + 1) + "</td>";
                            thu_str += calcularDataStr(thu[i].s);
                            thu_str += calcularDataStr(thu[i].e);
                            thu_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            fri_str += "<tr>";
                            fri_str += "<td>" + (i + 1) + "</td>";
                            fri_str += calcularDataStr(fri[i].s);
                            fri_str += calcularDataStr(fri[i].e);
                            fri_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            sat_str += "<tr>";
                            sat_str += "<td>" + (i + 1) + "</td>";
                            sat_str += calcularDataStr(sat[i].s);
                            sat_str += calcularDataStr(sat[i].e);
                            sat_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            sun_str += "<tr>";
                            sun_str += "<td>" + i + 1 + "</td>";
                            sun_str += calcularDataStr(sun[i].s);
                            sun_str += calcularDataStr(sun[i].e);
                            sun_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";
                        }
                        $("#p-tabs-week1 tbody").empty().append(mon_str);
                        $("#p-tabs-week2 tbody").empty().append(tue_str);
                        $("#p-tabs-week3 tbody").empty().append(wed_str);
                        $("#p-tabs-week4 tbody").empty().append(thu_str);
                        $("#p-tabs-week5 tbody").empty().append(fri_str);
                        $("#p-tabs-week6 tbody").empty().append(sat_str);
                        $("#p-tabs-week7 tbody").empty().append(sun_str);
                        child.showDay(); //显示当前星期
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setPlan: function () {
            var param = {};
            var con = {};
            var mon = new Array();
            var tue = new Array();
            var wed = new Array();
            var thu = new Array();
            var fri = new Array();
            var sat = new Array();
            var sun = new Array();

            $("#p-tabs-week1 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                mon.push(obj);
            });
            $("#p-tabs-week2 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                tue.push(obj);
            });
            $("#p-tabs-week3 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                wed.push(obj);
            });
            $("#p-tabs-week4 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                thu.push(obj);
            });
            $("#p-tabs-week5 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                fri.push(obj);
            });
            $("#p-tabs-week6 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                sat.push(obj);
            });
            $("#p-tabs-week7 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                sun.push(obj);
            });
            con.mon = mon;
            con.tue = tue;
            con.wed = wed;
            con.thu = thu;
            con.fri = fri;
            con.sat = sat;
            con.sun = sun;
            param.analysisplan = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/Plan?chnid=1",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
    }

}();
//算法参数
var sfcs = function () {
    function bindchangeEvent() {
        $('#sfcs-chepai-val').unbind('change');
        $('#sfcs-chepai-val').bind('change', function () {
            var value = parseInt($(this).val());
            var id = $(this).parent().prev().find("div").attr('id');
            if (value < 20 || value > 500) {
                alert('不合法的数据');
                $(this).val(0);
                $('#' + id).slider({
                    range: "min",
                    min: 20,
                    max: 500,
                    value: 0,
                    slide: function (event, ui) {
                        $(this).next().val(ui.value);
                    }
                });
                return;
            }

            $('#' + id).slider({
                range: "min",
                min: 20,
                max: 500,
                value: value,
                slide: function (event, ui) {
                    $(this).next().val(ui.value);
                }
            });
        });

        $('#sfcs-condition-val').unbind('change');
        $('#sfcs-condition-val').bind('change', function () {
            var value = parseInt($(this).val());
            var id = $(this).parent().prev().find("div").attr('id');
            if (value < 0 || value > 6) {
                alert('不合法的数据');
                $(this).val(0);
                $('#' + id).slider({
                    range: "min",
                    min: 0,
                    max: 6,
                    value: 0,
                    slide: function (event, ui) {
                        $(this).next().val(ui.value);
                    }
                });
                return;
            }

            $('#' + id).slider({
                range: "min",
                min: 0,
                max: 6,
                value: value,
                slide: function (event, ui) {
                    $(this).next().val(ui.value);
                }
            });
        });
    };

    return {
        addEvent: function () {
            child.addEvent();
            sfcs.init();
            bindchangeEvent();
            $(".dq_box li").click(function () {
                $(".dq_choice").removeClass("dq_choice");
                $(this).addClass("dq_choice");
            });

            $("#sfcs_form").validationEngine("attach", {
                promptPosition: "centerRight",
                scroll: false
            });
            $(".save-btn").click(function () {
                if (!$("#sfcs_form").validationEngine("validate")) {
                    return;
                }
                sfcs.setLPR();
            });

            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });

        },
        init: function () {
            sfcs.getLPR();
            sfcs.getDispatch();
            sfcs.getLocations();
        },
        getLPR: function () {
            $.ajax({
                url: "/ISAPI/Analysis/LPR",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.lpr;
                        var id = device.defprov;
                        var hanzi = device.hanzi; //汉字置信度 0-100
                        var minplatesize = device.minplatesize; //车牌最小尺寸20-500
                        var dupsens = device.dupsens; //多少字符相同判断为相同 0-6
                        $(".morenshengfen li[data-ty=pro_" + id + "]").addClass("dq_choice");
                        $('#sfcs-hanzi').slider({
                            range: "min",
                            min: 0,
                            max: 100,
                            value: hanzi,
                            slide: function (event, ui) {
                                $(this).parent().next().find('span').text(ui.value);
                                $(this).next().val(ui.value);
                            }
                        });
                        $('#sfcs-hanzi-text').text(hanzi);
                        $('#sfcs-hanzi').next().val(hanzi);

                        $('#sfcs-chepai').slider({
                            range: "min",
                            min: 20,
                            max: 500,
                            value: minplatesize,
                            slide: function (event, ui) {
                                $(this).parent().next().find('input').val(ui.value);
                                $(this).next().val(ui.value);
                            }
                        });
                        $('#sfcs-chepai-val').val(minplatesize);
                        $('#sfcs-chepai').next().val(minplatesize);

                        $('#sfcs-condition').slider({
                            range: "min",
                            min: 0,
                            max: 6,
                            value: dupsens,
                            slide: function (event, ui) {
                                $(this).parent().next().find('input').val(ui.value);
                                $(this).next().val(ui.value);
                            }
                        });
                        $('#sfcs-condition-val').val(dupsens);
                        $('#sfcs-condition').next().val(dupsens);

                        $("#sameCarSpace").val(device.duptime);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setLPR: function () {
            var param = {};
            var con = {};
            con.defprov = parseInt($(".dq_choice").attr("data-ty").split("_")[1]);
            con.hanzi = parseInt($("#sfcs-hanzi-text").text());
            con.minplatesize = parseInt($("#sfcs-chepai-val").val());
            con.dupsens = parseInt($("#sfcs-condition-val").val());
            con.duptime = parseInt($("#sameCarSpace").val());
            param.lpr = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Analysis/LPR",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        sfcs.setDispatch();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getDispatch: function () {
            $.ajax({
                url: "/ISAPI/Analysis/Dispatch",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.dispatch;
                        var sensitivity = device.sensitivity;
                        var priority = device.priority;
                        $('#sfcs-sensitivity').slider({
                            range: "min",
                            min: 0,
                            max: 2,
                            value: sensitivity,
                            slide: function (event, ui) {
                                $(this).next().val(ui.value);
                            }
                        });

                        $('#sfcs-sensitivity').next().val(sensitivity);

                        if (priority) {
                            $("#lxhf-wenjian-lx").prop("checked", false);
                            $("#lxhf-wenjian-tp").prop("checked", true);
                        } else {
                            $("#lxhf-wenjian-lx").prop("checked", true);
                            $("#lxhf-wenjian-tp").prop("checked", false);
                        }

                        $("#sfcs_stay").val(device.sceneminstay);
                        $("#repeat_snap").val(device.repeattime);

                        if (device.ptzlock) {
                            $(".a-offon").addClass("a-offon-click");
                        } else {
                            $(".a-offon").removeClass("a-offon-click");
                        }

                        $("#sfcs_locktime").val(device.locktime);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setDispatch: function () {
            var param = {};
            var con = {};
            con.sensitivity = parseInt($('#sfcs-sensitivity').next().val());
            con.priority = $("#lxhf-wenjian-lx").prop("checked") ? 0 : 1;
            con.sceneminstay = parseInt($("#sfcs_stay").val());
            con.repeattime = parseInt($("#repeat_snap").val());
            con.ptzlock = $(".a-offon").hasClass("a-offon-click") ? true : false;
            con.locktime = parseInt($('#sfcs_locktime').val());

            param.dispatch = con;
            param = JSON.stringify(param);

            $.ajax({
                url: "/ISAPI/Analysis/Dispatch",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        sfcs.setLocations();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getLocations: function () {
            $.ajax({
                url: "/ISAPI/Analysis/Locations?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var locations = data.locations;

                        $("#local_areaid").val(locations.areacode);
                        $("#local_areaname").val(locations.areaname);
                        $("#local_placeid").val(locations.placecode);
                        $("#local_placename").val(locations.placename);
                        $("#local_monitoryid").val(locations.monsitecode);
                        $("#local_monitoryname").val(locations.monsitename);
                       
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setLocations: function () {
            var param = {};
            var con = {};
            con.areacode = $('#local_areaid').val();
            con.areaname = $('#local_areaname').val();
            con.placecode = $('#local_placeid').val();
            con.placename = $('#local_placename').val();
            con.monsitecode = $('#local_monitoryid').val();
            con.monsitename = $('#local_monitoryname').val();
            param.locations = con;
            param = JSON.stringify(param);

            $.ajax({
                url: "/ISAPI/Analysis/Locations?chnid=1",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
    }
}();
//系统维护
var xtwh = function () {
    var rebootBari = 0;
    var reboottime = 0;
    var reboottime2 = 0;
    var rebootFactoryRestore = 0;

    function setProgressReboot(progress) {
        if (progress) {
            $('#reboot_span').text(progress);
        }
    };

    function setProgressFactoryRestore(progress) {
        if (progress) {
            $('#reboot_factoryRestore').text(progress);
        }
    };
    return {
        addEvent: function () {
            child.addEvent();
            xtwh.init();
            //开关按钮
            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });

            //            $(".shengji").click(function () {
            //                $(".shengji-c").css("display", "inline");
            //            });

            $(".addTimeBtn").click(function () {
                var str = '<li><input class="Wdate cq-time-input input-default" onfocus="WdatePicker({skin:\'whyGreen\',dateFmt:\'HH:mm\', readOnly:false,onpicked:function(){}})" value="00:00"  readonly type="text"><span class="cq-time-close">x</span></li>';
                $(".addNewTime").append(str);
                var currentLen = $(".addNewTime li").length;
                if (currentLen >= localStorage.maxRebootPoint) {
                    $(".addTimeBtn").hide();
                }
            });

            $(".addNewTime").on('click', '.cq-time-close', function () {
                $(this).parent().remove();
                var currentLen = $(".addNewTime li").length;
                if (currentLen < localStorage.maxRebootPoint) {
                    $(".addTimeBtn").show();
                }
            });

            $(".save-btn").click(function () {
                xtwh.setTimerReboot();
            });

            $(".huanyuan").click(function () {
                if ($(".wenjian").val() != '' && $(".wenjian").val() != null) {
                    xtwh.restoreMaintain();
                } else {
                    alert("请先选择文件!");
                    return;
                }
            });

            $(".shengji").click(function () { //升级
                if ($(".wenjian2").val() != '' && $(".wenjian2").val() != null) {
                    xtwh.upgradeMaintain();
                } else {
                    alert("请先选择文件!");
                    return;
                }
            });

            $(".ddcq").click(function () {
                xtwh.Reboot();
            });

            $(".huifu").click(function () {
                xtwh.setFactoryRestore();
            });

            $(".beifen").click(function () {
                xtwh.setMaintain();
            });

            $(".setAuth").click(function () {
                if ($(".license").val() != '' && $(".license").val() != null) {
                    xtwh.setSoftAuth();
                };
            });
            $(".delAuth").click(function () {
                if ($(".license").val() != '' && $(".license").val() != null) {
                    xtwh.delSoftAuth();
                }
            });
        },
        init: function () {
            xtwh.getTimerReboot();
            xtwh.getCapability();
            xtwh.getSoftAuth();
        },
        dieclick: function () {
            $(".ddcq,.huifu,.huanyuan,.shengji,.beifen").off("click");
        },
        getSoftAuth: function () {
            $.ajax({
                url: "/ISAPI/System/SoftAuth",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var re = data.softauth;
                        var type = re.authtype;
                        var timeout = re.timeout;
                        if (type == 2) {
                            type = '临时授权';
                            $('.auth_timeout').text(timeout);
                        } else {
                            type = type == 0 ? '未授权' : '永久授权';
                        }
                        $('.license').val(re.license);
                        $('.auth_type').text(type);

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        delSoftAuth: function () {
            $.ajax({
                url: "/ISAPI/System/SoftAuth",
                type: "DELETE",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        xtwh.getSoftAuth();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setSoftAuth: function () {
            var param = {};
            param.license = $('.license').val();
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/SoftAuth",
                type: "PUT",
                data: param,
                datatype: 'json',
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        xtwh.getSoftAuth();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setFactoryRestore: function () {
            $.ajax({
                url: "/ISAPI/System/Maintain/FactoryRestore",
                type: "PUT",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var re = data.reboot;
                        if (re.relogin) {
                            $(".huifu-c").css("display", "inline");
                            reboottime2 = re.waittime;
                            xtwh.dieclick();
                            xtwh.doProgressFactoryRestore();
                        }
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setMaintain: function () { //保存备份
            $.ajax({
                url: "/ISAPI/System/Maintain/Profile",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        // $(".beifen-c").css("display", "inline"); 
                        var url = data.export.filename;
                        window.open(url, '_blank');
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        restoreMaintain: function () { //还原备份
            var fdata = new FormData($("#postStore")[0]);
            //console.log($("#postStore")[0] == null);
            // console.log($("#postStore").serialize());
            $.ajax({
                url: "/ISAPI/System/Maintain/Profile",
                type: "POST",
                async: false,
                cache: false,
                contentType: false,
                processData: false,
                data: fdata,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        // $(".beifen-c").css("display", "inline");
                        if (confirm("是否重启生效?")) {
                            xtwh.Reboot();
                        }
                    } else {
                        alert("还原备份失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        upgradeMaintain: function () { //升级备份
            var fdata = new FormData($("#upgradeStore")[0]);
            //console.log($("#postStore")[0] == null);
            // console.log($("#postStore").serialize());
            $.ajax({
                url: "/ISAPI/System/Maintain/Upgrade",
                type: "POST",
                async: false,
                cache: false,
                contentType: false,
                processData: false,
                data: fdata,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        // $(".beifen-c").css("display", "inline");
                        if (confirm("是否重启生效?")) {
                            xtwh.Reboot();
                        }
                    } else {
                        alert("升级系统失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getCapability: function () {
            $.ajax({
                url: "/ISAPI/System/Maintain/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.storeStrategy;
                        localStorage.maxRebootPoint = device.maxRebootPoint;
                        localStorage.isSupportUpgrade = device.isSupportUpgrade;
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getTimerReboot: function () {
            $.ajax({
                url: "/ISAPI/System/Maintain/TimerReboot",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.timerReboot;
                        if (device.enable) {
                            $(".a-offon").addClass('a-offon-click');
                        } else {
                            $(".a-offon").removeClass('a-offon-click');
                        }
                        if (device.timerlist) {
                            var str = '';
                            var timeArray = device.timerlist;
                            for (var i = 0; i < device.timerlist.length; i++) {
                                var timeMessage = '';
                                var hour = parseInt(timeArray[i] / 3600);
                                var minute = (timeArray[i] % 3600 / 60);
                                if (hour < 10) {
                                    hour = '0' + hour;
                                }
                                if (minute < 10) {
                                    minute = '0' + minute;
                                }
                                timeMessage = hour + ':' + minute;
                                str += '<li><input class="Wdate cq-time-input input-default" onfocus="WdatePicker({skin:\'whyGreen\',dateFmt:\'HH:mm\', readOnly:false,onpicked:function(){}})" value="' + timeMessage + '"  readonly type="text"><span class="cq-time-close">x</span></li>';
                            }
                            $(".addNewTime").empty().append(str);
                            if ($(".addNewTime li").length >= localStorage.maxRebootPoint) {
                                $(".addTimeBtn").hide();
                            }
                        }
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setTimerReboot: function () {
            var param = {};
            var con = {};
            var timerlist = [];
            if ($(".a-offon").hasClass('a-offon-click')) {
                con.enable = true;
            } else {
                con.enable = false;
            }

            $(".addNewTime li").each(function (index) {
                var time = $(this).find("input").val();
                var timeSplit = time.split(':');
                timerlist[index] = parseInt(timeSplit[0]) * 3600 + parseInt(timeSplit[1] * 60);
            });
            con.timerlist = timerlist;
            param.timerReboot = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Maintain/TimerReboot",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        Reboot: function () {
            $.ajax({
                url: "/ISAPI/System/Maintain/Reboot",
                type: "PUT",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var re = data.reboot;
                        if (re.relogin) {
                            $(".ddcq-c").css("display", "inline");
                            reboottime = re.waittime;
                            xtwh.dieclick();
                            xtwh.doProgressRebootDevice();
                        }

                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        doProgressRebootDevice: function () {
            if (rebootBari > 100) {
                rebootBari = 0;
                self.location = 'login.html';
                return;
            } else {
                setTimeout("xtwh.doProgressRebootDevice()", 1000);
                setProgressReboot(rebootBari);
                rebootBari += Math.round(100 / reboottime);
            }
        },
        doProgressFactoryRestore: function () {
            if (rebootFactoryRestore > 100) {
                rebootFactoryRestore = 0;
                self.location = 'login.html';
                return;
            } else {
                setTimeout("xtwh.doProgressFactoryRestore()", 1000);
                setProgressFactoryRestore(rebootFactoryRestore);
                rebootFactoryRestore += Math.round(100 / reboottime);
            }
        },
    }
}();
//用户管理
var yhgl = function () {
    function getUserLevel(num) {
        var str = "";
        if (num == 0) {
            str = '管理员';
        } else if (num == 1) {
            str = '操作员';
        } else {
            str = '观察员';
        }
        return str;
    };

    function getUserRight(num) {
        var str = "";
        if (localStorage.unnn == "admin") {
            if (num == 0) {
                str = '<a class="icon-cz icon-xg" title="修改"></a>';
            } else {
                str = '<a class="icon-cz icon-xg" title="修改"></a><a class="icon-cz icon-sc" title="删除"></a>';
            }
        } else {
            str = '<a class="icon-cz icon-xg" title="修改"></a>';
        }
        return str;
    };

    function addSameUser() {
        var newUser = $('#addusername').val();
        var count = 0;
        $('#userlist').find('tr').each(function () {
            if (newUser == $(this).children('td').eq(1).html()) {
                count++;
            }
        });
        if (count > 0) {
            return true;
        } else {
            return false;
        }
    };

    return {
        addEvent: function () {
            child.addEvent();
            yhgl.init();
            $(".addUser").click(function () {
                $("#addNewUser").show();
            });
            $(".addUserCancel,.a-btn-qx").click(function () {
                $(".uempty").val('');
                $("#addNewUser").hide();
            });
            $(".eidtUserCancel").click(function () {
                $(".uempty").val('');
                $("#editNewUser").hide();
            });
            $("#editSaveUser").click(function () {
                yhgl.editUser();
            });
            $("#addSaveUser").click(function () {
                var ulen = $("#userlist").find("tr").length;
                var maxnum = yhgl.getUserCapabilities();
                if (ulen >= maxnum) {
                    alert('添加用户达到上限!');
                    return false;
                } else {
                    yhgl.addUser();
                }
            });
            $("#userlist").on('click', '.icon-sc', function () {
                if (localStorage.unnn == "admin") {
                    $("#deleteUser").show();
                    var uname = $(this).parent().attr("data-nav");
                    $("#deluserID").val(uname);

                } else {
                    alert("no permission!");
                }
            });
            $("#delUser").click(function () {
                var uname = $("#deluserID").val();
                yhgl.delUser(uname);
            });
            $(".delCancel").click(function () {
                $("#deluserID").val('');
                $("#deleteUser").hide();
            });
            $("#userlist").on('click', '.icon-xg', function () { //修改
                var u = localStorage.unnn;
                var uname = $(this).parent().attr("data-nav");

                if (u == "admin") { //修改的用户是管理员
                    if (uname == "admin") { //修改自己
                        $("#editUserLevel").empty().append("<option value='0'>管理员</option>");
                        yhgl.beforeEditUser($(this));
                        $("#editNewUser").show();
                    } else { //修改非管理员 
                        if ($(this).parent().prev().attr("data-lev") == 1) {
                            $("#editUserLevel").empty().append("<option value='1' selected>操作员</option><option value='2'>观察员</option>");
                        } else {
                            $("#editUserLevel").empty().append("<option value='1'>操作员</option><option value='2' selected>观察员</option>");
                        }
                        yhgl.beforeEditUser($(this));
                        $("#editNewUser").show();
                    }
                } else { //修改者是非管理员
                    if (uname != u) {
                        alert("只能修改自己的权限！");
                        return;
                    } else {
                        var lev = $(this).parent().prev().attr("data-lev");
                        if (lev == 1) {
                            $("#editUserLevel").empty().append("<option value='1'>操作员</option>");
                        } else if (lev == 2) {
                            $("#editUserLevel").empty().append("<option value='2'>观察员</option>");
                        }
                        yhgl.beforeEditUser($(this));
                        $("#editNewUser").show();
                    }
                }
            });

        },
        init: function () {
            var u = localStorage.unnn;
            if (u == 'admin') {
                $('.addUser').show();
            } else {
                $('.addUser').hide();
            }
            yhgl.getAllUser();
        },
        beforeEditUser: function (obj) {
            $(".uempty").empty();
            $("#editUname").val(obj.parent().attr("data-nav"));
            $("#editRemark").val(obj.parent().attr("data-remark"));
        },
        getAllUser: function () {
            $.ajax({
                url: "/ISAPI/Security/Users",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.userlist;
                        var str = "";
                        for (var k = 0; k < device.length; k++) {
                            str += '<tr><td>' + (k + 1) + '</td>';
                            str += '<td>' + device[k].name + '</td>';
                            str += '<td data-lev=' + device[k].userLevel + '>' + getUserLevel(device[k].userLevel) + '</td>';
                            str += '<td data-nav=' + device[k].name + ' data-remark=' + device[k].remark + '>' + getUserRight(device[k].userLevel) + '</td>';
                            str += '</tr>';
                        }
                        if (localStorage.unnn == 'admin') {} else { //非admin用户无权限添加用户
                            $("#addSaveUser").hide();
                        }
                        $("#userlist").empty().append(str);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        addUser: function () {
            if (!$("#addUserForm").validationEngine("validate")) {
                return;
            }
            if ($('#addpasswd1').val() == $('#addpasswd2').val()) {} else {
                $("#addpasswd2").validationEngine("showPrompt", "请输入与上面相同的密码", "error");
                return;
            }
            if (addSameUser()) {
                alert('用户已存在!')
                return false;
            }
            var param = {};
            var con = {};
            con.name = $("#addusername").val();
            var b = new Base64();
            var encodePwd = b.encode($("#addpasswd1").val());
            con.password = encodePwd;
            con.remark = $("#addRemark").val();
            con.userLevel = parseInt($("#addUserLevel").val());
            param.user = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Security/User",
                type: "POST",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".uempty").val('');
                        $("#addNewUser").hide();
                        yhgl.getAllUser();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        editUser: function () {
            if (!$("#editUserForm").validationEngine("validate")) {
                return;
            }
            if ($('#editpasswd1').val() == $('#editpasswd1').val()) {} else {
                $("#editpasswd1").validationEngine("showPrompt", "请输入与上面相同的密码", "error");
                return;
            }
            if (addSameUser()) {
                alert('用户已存在!')
                return false;
            }
            var param = {};
            var con = {};
            con.name = $("#editUname").val();
            var b = new Base64();
            var encodePwd = b.encode($("#editpasswd1").val());
            con.password = encodePwd;
            con.remark = $("#editRemark").val();
            con.userLevel = parseInt($("#editUserLevel").val());
            param.user = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Security/User",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".uempty").val('');
                        $("#editNewUser").hide();
                        yhgl.getAllUser();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getUserCapabilities: function () {
            var num = 0;
            $.ajax({
                url: "/ISAPI/Security/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        num = data.securityCap.maxUserNum;
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
            return num;
        },
        delUser: function (u) {
            $.ajax({
                url: "/ISAPI/Security/User?name=" + u,
                type: "DELETE",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $("#deluserID").val('');
                        $("#deleteUser").hide();
                        yhgl.getAllUser();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }
}();
//磁盘管理
var cpgl = function () {
    function devtype(num) {
        var str = "";
        if (num == 0) {
            str = "ROM";
        } else if (num == 1) {
            str = "SD";
        } else if (num == 2) {
            str = "USB";
        } else {
            str = "HDD";
        }
        return str;
    };

    function usages(num0, num) {
        var str = "";
        if (num == 0) {
            str = "系统";
        }
        var str2 = "";

        if (num0 == 1 || num0 == 2) {
            if (num == 1) {
                str2 = '<select name="" id="" class="select-default" style="width: 150px;"><option value="1" selected>不使用</option>' +
                    '<option value="2">存储数据</option><option value="3">离线拷贝</option></select>';
            } else if (num == 2) {
                str2 = '<select name="" id="" class="select-default" style="width: 150px;"><option value="1">不使用</option>' +
                    '<option value="2" selected>存储数据</option><option value="3">离线拷贝</option></select>';
            } else if (num == 3) {
                str2 = '<select name="" id="" class="select-default" style="width: 150px;"><option value="1">不使用</option>' +
                    '<option value="2">存储数据</option><option value="3" selected>离线拷贝</option></select>';
            }
        }
        if (num0 == 3 && num == 1) {
            str2 = '<select name="" id="" class="select-default" style="width: 150px;"><option value="1" selected>不使用</option><option value="2">存储数据</option></select>';
        } else if (num0 == 3 && num == 2) {
            str2 = '<select name="" id="" class="select-default" style="width: 150px;"><option value="1">不使用</option><option value="2" selected>存储数据</option></select>';
        }

        return str + str2;
    };

    function status(num) {
        var str;
        if (num == 0) {
            str = "正常工作";
        } else if (num == 1) {
            str = "未发现设备";
        } else if (num == 2) {
            str = "设备不可用";
        } else {
            str = "未挂载";
        }
        return str;
    };

    return {
        addEvent: function () {
            child.addEvent();
            cpgl.init();
            $("#diskTable").on("click", '.icon-geshihua', function () {
                var devtype = $(this).attr("data-devtype");
                cpgl.setFormatDisk(devtype);
            });
            $(".save-btn").click(function () {
                cpgl.setStoreDisks();
            });
        },
        init: function () {
            cpgl.getStoreDisks();
            cpgl.getFullStrategy();
        },
        getStoreDisks: function () {
            $.ajax({
                url: "/ISAPI/System/Store/Disks",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.disks;
                        var str = "";
                        for (var i = 0; i < device.length; i++) {
                            str += "<tr><td>" + (i + 1) + "</td>";
                            str += "<td data-type=" + device[i].disktype + ">" + devtype(device[i].disktype) + "</td>";
                            str += "<td>" + device[i].total + "</td>";
                            str += "<td>" + device[i].free + "</td>";
                            str += "<td>" + device[i].format + "</td>";
                            str += "<td data-usage=" + device[i].usage + ">" + usages(device[i].disktype, device[i].usage) + "</td>";
                            str += "<td data-stat=" + device[i].stat + ">" + status(device[i].stat) + "</td>";

                            if (device[i].disktype != 0 && device[i].stat != 1 && device[i].stat != 2) {
                                str += "<td><a class='icon-cz icon-geshihua' data-devtype=" + device[i].disktype + " title='格式化'></a></td></tr>";
                            } else {
                                str += "<td></td></tr>";
                            }
                        }
                        $("#diskTable").empty().append(str);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setStoreDisks: function () {
            var param = {};
            var con = [];
            $("#diskTable").find("tr").each(function (index) {
                var obj = {};
                obj.disktype = parseInt($(this).find("td:eq(1)").attr("data-type"));
                if (obj.disktype == 0) {} else {
                    obj.format = $(this).find("td:eq(4)").text();
                    obj.free = $(this).find("td:eq(3)").text();
                    obj.stat = parseInt($(this).find("td:eq(6)").attr("data-stat"));
                    obj.total = $(this).find("td:eq(2)").text();
                    obj.usage = parseInt($(this).find("td:eq(5) select").val());
                    con.push(obj);
                }
            });

            param.disks = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Store/Disks",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        cpgl.setFullStrategy();
                    } else {
                        alert("设置信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getFullStrategy: function () {
            $.ajax({
                url: "/ISAPI/System/Store/FullStrategy",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.storeStrategy;
                        $("#threshold_cpgl").val(device.threshold);
                        $("#storeStrategy_cpgl").val(device.strategy);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setFullStrategy: function () {
            var param = {};
            var con = {};
            con.threshold = parseInt($("#threshold_cpgl").val());
            con.strategy = parseInt($("#storeStrategy_cpgl").val());
            param.storeStrategy = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Store/FullStrategy",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert("设置信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setFormatDisk: function (num) {
            var devtype = parseInt(num);
            $.ajax({
                url: "/ISAPI/System/Store/FormatDisk?disktype=" + devtype,
                type: "PUT",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        cpgl.init();
                    } else {
                        alert("格式化失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }
}();

//录像管理
var lxgl = function () {
    function calcularTime(time) {
        var str;
        if (time <= 60) {
            if (time < 10) {
                time = "0" + time
            };
            str = "00:00:" + time;
        } else if (time > 60 && time < 3600) {
            var m = Math.floor(time / 60);
            var s = time - Math.floor(time / 60) * 60;
            if (m < 10) {
                m = "0" + m;
            }
            if (s < 10) {
                s = "0" + s;
            }
            str = "00:" + m + ":" + s;
        } else {
            var h = Math.floor(time / 3600);
            var m = Math.floor((time - h * 3600) / 60);
            var s = time - h * 3600 - m * 60;
            if (h < 10) {
                h = "0" + h;
            }
            if (m < 10) {
                m = "0" + m;
            }
            if (s < 10) {
                s = "0" + s;
            }
            str = h + ":" + m + ":" + s;
        }

        return str;
    };

    function parseToSecond(s) {
        var sp = s.split(":");
        return parseInt(sp[0]) * 3600 + parseInt(sp[1]) * 60 + parseInt(sp[2]);
    }

    function calcularDataStr(time) {
        var str = "";
        str += '<td><input style="width: 140px;height: 28px;margin:0 auto;"class="Wdate input-default" type="text" onfocus="WdatePicker({skin:\'whyGreen\',dateFmt:\'HH:mm:ss\', readOnly:false,onpicked:function(){}})" value=' + calcularTime(time) + ' readonly/></td>';
        return str;
    }
    return {
        addEvent: function () {
            child.addEvent();
            lxgl.init();
            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });
            $('.save-btn').click(function () {
                lxgl.setRecordSwitch();
            });

            $("#zpjs-quanbu").click(function () {
                $(".weekbox").prop("checked", true);
                $("#zpjs-fanxiang").prop("checked", false);
            });

            $("#zpjs-fanxiang").click(function () {
                $("#zpjs-quanbu").prop("checked", false);
                $('.weekbox').each(function () {
                    if ($(this).prop("checked")) {
                        $(this).prop("checked", false);
                    } else {
                        $(this).prop("checked", true);
                    }
                });
            });

            $(".setFirstRecord").click(function () {
                var week = $(".jh-tab").find(".active").attr("aweek"); //设置周几
                var tr1 = $("#p-tabs-week" + week).find("tbody tr")[0];
                $(tr1).find("td:eq(1) input").val("00:00:00");
                $(tr1).find("td:eq(2) input").val("23:59:59");
            });

            $(".tab_content tbody").on('click', '.cleardata', function () {
                $(this).parent().prev().find("input").val("00:00:00");
                $(this).parent().prev().prev().find("input").val("00:00:00");
            });

            $(".copyToWeek").click(function () {
                var week = $(".jh-tab").find(".active").attr("aweek"); //copy样本
                var clone = '';
                $(".weekbox").each(function () {
                    if ($(this).prop("checked")) {
                        var cloneTo = $(this).attr("data-aweek");
                        if (week == cloneTo) { //复制到的星期和当前星期一致时不需要复制

                        } else {
                            clone = $('#p-tabs-week' + week).contents().clone(true);
                            $('#p-tabs-week' + cloneTo).empty().append(clone);
                            clone = '';
                        }
                    }
                });
            });
        },
        init: function () {
            lxgl.getRecordStategy();
            lxgl.getRecordSwitch();
            lxgl.getRecordCapabilities();
        },
        getRecordSwitch: function () {
            $.ajax({
                url: "/ISAPI/Record/Switch?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.switch;
                        if (device) {
                            $(".a-offon").addClass("a-offon-click");
                        } else {
                            $(".a-offon").removeClass("a-offon-click");
                        }
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setRecordSwitch: function () {
            var switchs = false;
            if ($(".a-offon").hasClass("a-offon-click")) {
                switchs = true;
            }
            var param = {
                "switch": switchs
            };
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Record/Switch?chnid=1",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        lxgl.setRecordStategy();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getRecordStategy: function () {
            $.ajax({
                url: "/ISAPI/Record/Strategy",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.recordStrategy;
                        if (device.packetmode == 0) {
                            $("#lxglSize").prop("checked", true);
                            $("#lxglTime").prop("checked", false);
                        } else {
                            $("#lxglSize").prop("checked", false);
                            $("#lxglTime").prop("checked", true);
                        }
                        //  console.log($("#lxglSize").prop('checked'));
                        $("#lxgl_packetsize").val(device.packetsize);
                        $("#lxgl_packettime").val(device.packettime);
                        $("#lxgl_fileformat").val(device.fileformat);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setRecordStategy: function () {
            var param = {};
            var con = {};
            var packsize = parseInt($("#lxgl_packetsize").val());
            var packettime = parseInt($("#lxgl_packettime").val());
            var fileformat = parseInt($("#lxgl_fileformat").val());
            if (isNaN(packsize)) packsize = 0;
            if (isNaN(packettime)) packettime = 0;
            if (isNaN(fileformat)) fileformat = 0;
            if ($("#lxgl-dabao-daxiao").prop("checked")) {
                con.packetmode = 0;
            } else {
                con.packetmode = 1;
            }
            con.packetsize = packsize;
            con.packettime = packettime;
            con.fileformat = fileformat;
            param.recordStrategy = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Record/Strategy",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        lxgl.setRecordPlan();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getRecordCapabilities: function () {
            $.ajax({
                url: "/ISAPI/Record/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.recordCap;
                        var maxPlanInDay = device.maxPlanInDay; //一天最多配置多少个时间段
                        lxgl.getRecordPlan(maxPlanInDay);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getRecordPlan: function (maxPlan) {
            $.ajax({
                url: "/ISAPI/Record/Plan?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.recordplan;
                        var mon_str, tue_str, wed_str, thu_str, fri_str, sat_str, sun_str;
                        var mon = device.mon;
                        var tue = device.tue;
                        var wed = device.wed;
                        var thu = device.thu;
                        var fri = device.fri;
                        var sat = device.sat;
                        var sun = device.sun;
                        for (var i = 0; i < maxPlan; i++) {
                            mon_str += "<tr>";
                            mon_str += "<td>" + (i + 1) + "</td>";
                            mon_str += calcularDataStr(mon[i].s);
                            mon_str += calcularDataStr(mon[i].e);
                            mon_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            tue_str += "<tr>";
                            tue_str += "<td>" + (i + 1) + "</td>";
                            tue_str += calcularDataStr(tue[i].s);
                            tue_str += calcularDataStr(tue[i].e);
                            tue_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            wed_str += "<tr>";
                            wed_str += "<td>" + (i + 1) + "</td>";
                            wed_str += calcularDataStr(wed[i].s);
                            wed_str += calcularDataStr(wed[i].e);
                            wed_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            thu_str += "<tr>";
                            thu_str += "<td>" + (i + 1) + "</td>";
                            thu_str += calcularDataStr(thu[i].s);
                            thu_str += calcularDataStr(thu[i].e);
                            thu_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            fri_str += "<tr>";
                            fri_str += "<td>" + (i + 1) + "</td>";
                            fri_str += calcularDataStr(fri[i].s);
                            fri_str += calcularDataStr(fri[i].e);
                            fri_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            sat_str += "<tr>";
                            sat_str += "<td>" + (i + 1) + "</td>";
                            sat_str += calcularDataStr(sat[i].s);
                            sat_str += calcularDataStr(sat[i].e);
                            sat_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";

                            sun_str += "<tr>";
                            sun_str += "<td>" + i + 1 + "</td>";
                            sun_str += calcularDataStr(sun[i].s);
                            sun_str += calcularDataStr(sun[i].e);
                            sun_str += "<td><a class='icon-cz icon-sc cleardata' title='删除时间'></a></td></tr>";
                        }
                        $("#p-tabs-week1 tbody").empty().append(mon_str);
                        $("#p-tabs-week2 tbody").empty().append(tue_str);
                        $("#p-tabs-week3 tbody").empty().append(wed_str);
                        $("#p-tabs-week4 tbody").empty().append(thu_str);
                        $("#p-tabs-week5 tbody").empty().append(fri_str);
                        $("#p-tabs-week6 tbody").empty().append(sat_str);
                        $("#p-tabs-week7 tbody").empty().append(sun_str);
                        child.showDay();
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setRecordPlan: function () {
            var param = {};
            var con = {};
            var mon = new Array();
            var tue = new Array();
            var wed = new Array();
            var thu = new Array();
            var fri = new Array();
            var sat = new Array();
            var sun = new Array();

            $("#p-tabs-week1 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                mon.push(obj);
            });
            $("#p-tabs-week2 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                tue.push(obj);
            });
            $("#p-tabs-week3 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                wed.push(obj);
            });
            $("#p-tabs-week4 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                thu.push(obj);
            });
            $("#p-tabs-week5 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                fri.push(obj);
            });
            $("#p-tabs-week6 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                sat.push(obj);
            });
            $("#p-tabs-week7 tbody").find("tr").each(function (index) {
                var obj = {};
                var s = $(this).find("td:eq(1) input").val();
                var e = $(this).find("td:eq(2) input").val();
                obj.s = parseToSecond(s);
                obj.e = parseToSecond(e);
                sun.push(obj);
            });
            con.mon = mon;
            con.tue = tue;
            con.wed = wed;
            con.thu = thu;
            con.fri = fri;
            con.sat = sat;
            con.sun = sun;
            param.recordplan = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Record/Plan?chnid=1",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }

    }
}();

//上传配置
var scpz = function () {
    return {
        addEvent: function () {
            child.addEvent();
            scpz.init();
            $(".a-offon").click(function () {
                $(".a-offon").removeClass("a-offon-click");
                $(this).addClass("a-offon-click");
            });
            $(".save-btn").click(function () {
                //    scpz.setUploadSwitch();
                scpz.setUploadStrategy();
                scpz.setFtpAdance();
                scpz.getNotifyCapabilities(0);
            });

            $("#switchAll").click(function () {
                $("#switchAll").toggleClass("a-offon-click");
                scpz.setUploadSwitch();
            });

            $("#notifySwitch").click(function () {
                $("#notifySwitch").toggleClass("a-offon-click");
                scpz.setNotifySwitch();
            });
        },
        init: function () {
            scpz.getUploadSwitch();
            scpz.getUploadStrategy();
            scpz.getFtpAdance();
            scpz.getNotifyCapabilities(1);
        },
        getNotifyCapabilities: function (num) {
            $.ajax({
                url: "/ISAPI/Notify/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var content = data.notifyCap;
                        if (content.isSupportWeChat) {
                            if (num) {
                                $(".nodifyFunc").show();
                                scpz.getNotifySwitch();
                                scpz.getNotifyStrategy();
                            } else {
                                scpz.setNotifyStrategy();
                            }
                        } else {
                            $(".nodifyFunc").hide();
                        }

                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNotifySwitch: function () {
            $.ajax({
                url: "/ISAPI/Notify/Switch",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var content = data.switch;
                        if (content) {
                            $("#notifySwitch").addClass("a-offon-click");
                        } else {
                            $("#notifySwitch").removeClass("a-offon-click");
                        }

                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNotifySwitch: function () {
            var switchs = false;
            if ($("#notifySwitch").hasClass("a-offon-click")) {
                switchs = true;
            }
            var param = {
                "switch": switchs
            };
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Notify/Switch",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                    } else {
                        alert("保存信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNotifyStrategy: function () {
            $.ajax({
                url: "/ISAPI/Notify/Strategy",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var content = data.notify;
                        localStorage.notifyPort = content.port;
                        $("#notifyAddr").val(content.address);
                        $("#addImage").prop('checked', content.attachimage);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setNotifyStrategy: function () {
            var param = {};
            var con = {};
            con.address = $("#notifyAddr").val();
            con.attachimage = $("#addImage").prop('checked');
            con.port = parseInt(localStorage.notifyPort);
            param.notify = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Notify/Strategy",
                type: "PUT",
                cache: false,
                async: false,
                data: param,
                datatype: 'json',
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {

                        $(".save-successful").show();
                    } else {
                        alert("保存失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getUploadStrategy: function () {
            $.ajax({
                url: "/ISAPI/Upload/Strategy",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var content = data.upload;
                        if (content.behavior.deletesuccess) {
                            $("#deletesuccess").prop("checked", true);
                        } else {
                            $("#deletesuccess").prop("checked", false);
                        }
                        if (content.uploadway == 0) {
                            $("#uploadByFTP").addClass("a-offon-click");
                        } else if (content.uploadway == 1) {
                            $("#uploadByPlatform").addClass("a-offon-click");
                        }
                        else if (content.uploadway == 2) {
                            $("#uploadByMQTT").addClass("a-offon-click");
                        }
                        var ftp = content.ftp;
                        $("#ftpAddr").val(ftp.address);
                        $("#ftpPort").val(ftp.port);
                        $("#ftpUname").val(ftp.user);
                        $("#ftpPwd").val(ftp.password);

                        var platform = content.platform;
                        $("#platAddr").val(platform.address);
                        $("#platPort").val(platform.port);
                        $("#platNumber").val(platform.id);

                        var mqtt = content.mqtt;
                        $("#mqttAddr").val(mqtt.address);
                        $("#mqttPort").val(mqtt.port);
                        $("#mqttNumber").val(mqtt.id);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setUploadStrategy: function () {
            var param = {};
            var con = {};
            var ftp = {};
            var plat = {};
            var mqtt = {};
            var beh = {};

            beh.deletesuccess = $("#deletesuccess").prop("checked");
            con.behavior = beh;

            ftp.address = $("#ftpAddr").val();
            ftp.port = parseInt($("#ftpPort").val());
            ftp.user = $("#ftpUname").val();
            ftp.password = $("#ftpPwd").val();
            con.ftp = ftp;

            plat.address = $("#platAddr").val();
            plat.port = parseInt($("#platPort").val());
            plat.id = $("#platNumber").val();
            con.platform = plat;

            mqtt.address = $("#mqttAddr").val();
            mqtt.port = parseInt($("#mqttPort").val());
            mqtt.id = $("#mqttNumber").val();
            con.mqtt = mqtt;
            
            if ($("#uploadByFTP").hasClass("a-offon-click")) {
                con.uploadway = 0;
            } else if ($("#uploadByPlatform").hasClass("a-offon-click")) {
                con.uploadway = 1;
            } else {
                con.uploadway = 2;
            }

            param.upload = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Upload/Strategy",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        $(".save-successful").hide();
                        alert("设置信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getUploadSwitch: function () {
            $.ajax({
                url: "/ISAPI/Upload/Switch",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        if (data.switch) {
                            $("#switchAll").addClass("a-offon-click");
                        } else {
                            $("#switchAll").removeClass("a-offon-click");
                        }
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setUploadSwitch: function () {
            var switchs = false;
            if ($("#switchAll").hasClass("a-offon-click")) {
                switchs = true;
            }
            var param = {
                "switch": switchs
            };
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Upload/Switch",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        //$(".save-successful").show();
                        //                        if (data.switch) {
                        //                            $("#switchAll").addClass("a-offon-click");
                        //                        } else {
                        //                            $("#switchAll").removeClass("a-offon-click");
                        //                        }
                    } else {
                        alert("保存信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getFtpAdance: function () {
            $.ajax({
                url: "/ISAPI/Upload/FtpAdvance",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var content = data.ftpadvance;
                        if (content.textflag) {
                            $("#textflag").prop("checked", true);
                        } else {
                            $("#textflag").prop("checked", false);
                        }
                        if (content.imageflag) {
                            $("#imageflag").prop("checked", true);
                        } else {
                            $("#imageflag").prop("checked", false);
                        }
                        if (content.recordflag) {
                            $("#recordflag").prop("checked", true);
                        } else {
                            $("#recordflag").prop("checked", false);
                        }

                        $("#charsettype").val(content.namecharset);                        
                        $("#illegalgrammar").val(content.illegalgrammar);
                        $("#eventgrammar").val(content.eventgrammar);
                        $("#tollgrammar").val(content.tollgrammar);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setFtpAdance: function () {
            var param = {};
            var ftp = {};

            ftp.textflag = $("#textflag").prop("checked");
            ftp.imageflag = $("#imageflag").prop("checked");
            ftp.recordflag = $("#recordflag").prop("checked");
            ftp.namecharset = parseInt($("#charsettype").val());
            ftp.illegalgrammar = $("#illegalgrammar").val();
            ftp.eventgrammar = $("#eventgrammar").val();
            ftp.tollgrammar = $("#tollgrammar").val();
            param.ftpadvance = ftp;
          
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/Upload/FtpAdvance",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        $(".save-successful").hide();
                        alert("设置信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }
}();

//时间配置
var sjpz = function () {
    function getComputerTime() {
        var mydate = new Date();
        var str = "" + mydate.getFullYear() + '-';
        str += (mydate.getMonth() + 1) + "-";
        str += mydate.getDate() + " ";
        str += mydate.getHours() + ":";
        str += mydate.getMinutes() + ":";
        str += mydate.getSeconds();
        return str;
    }
    return {
        addEvent: function () {
            child.addEvent();
            sjpz.init();
            $("#putUserTime").click(function () {
                sjpz.setTimeStatus(0);
            });
            $("#setComputerTime").click(function () {
                sjpz.setTimeStatus(1);
            });

            $(".a-offon").click(function () {
                if ($(this).hasClass("a-offon-click")) {
                    $(this).toggleClass("a-offon-click");
                } else {
                    $(".a-offon").removeClass("a-offon-click");
                    $(this).toggleClass("a-offon-click");
                }
            });
            $(".save-btn").click(function () {
                sjpz.setSynMechanism();
            });
        },
        init: function () {
            sjpz.getTimeStatus();
            sjpz.getSynMechanism();
        },
        getTimeStatus: function () {
            $.ajax({
                url: "/ISAPI/System/Time/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $('#localtime').val(data.localtime);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setTimeStatus: function (flag) {
            var localtime = '';
            if (flag) {
                localtime = getComputerTime();
                $('#localtime').val(localtime);
            } else {
                localtime = $('#localtime').val() + '';
            }
            var param = {
                "localtime": localtime
            };
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Time/Status",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $('#localtime').val(localtime);
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getSynMechanism: function () {
            $.ajax({
                url: "/ISAPI/System/Time/SynMechanism",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $("#addrNTP").val(data.timeSyn.ntpaddr);
                        $("#intevalTime").val(data.timeSyn.syncycle);
                        $("#synchid").val(data.timeSyn.synchnid);
                        var type = data.timeSyn.syntype;
                        $(".a-offon").removeClass("a-offon-click");
                        if (type) {
                            $(".a-offon").each(function () {
                                if ($(this).attr("data-on") == type) {
                                    $(this).addClass("a-offon-click");
                                };
                            })
                        }
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setSynMechanism: function () {
            var param = {};
            var con = {};
            con.ntpaddr = $("#addrNTP").val();
            con.syncycle = parseInt($("#intevalTime").val());
            con.synchnid = parseInt($("#synchid").val());
            con.syntype = parseInt($(".a-offon-click").attr("data-on")) ? parseInt($(".a-offon-click").attr("data-on")) : 0;
            param.timeSyn = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/Time/SynMechanism",
                type: "PUT",
                data: param,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").show();
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        }
    }
}();

//设备信息
var sbxq = function () {
    function loadPlugin() {
        var plugin = document.getElementById("plugin2");
        if (plugin.valid) {
            var path1 = plugin.GetSaveDirectory(0); //截图
            var path2 = plugin.GetSaveDirectory(1); //录像
            $("#picPath").val(path1);
            $("#recordPath").val(path2);
        }

    };

    //浏览目录 cmd=0选取截取图片存放路径   cmd=1选取录像存储路径
    function browseDirectories(cmd) {
        var plugin = document.getElementById("plugin2");
        var path = plugin.BrowseDirectories(cmd);
        if (0 == cmd) {
            $("#picPath").val(path);
        } else if (1 == cmd) {
            $("#recordPath").val(path);
        }
    };
    return {
        addEvent: function () {
            child.addEvent();
            sbxq.init();
            $('.save-btn').click(function () {
                sbxq.setDeviceInfo();
            });
            $('#yulan_cclj').click(function () {
                browseDirectories(0);
            });
            $('#yulan_lxlj').click(function () {
                browseDirectories(1);
            });
        },
        init: function () {
            sbxq.getDeviceInfo();
            sbxq.getGPSStatus();
            sbxq.getSystemStatus();
            sbxq.getNetworkStatus();
            sbxq.getStoreStatus();
            sbxq.getVideoInStatus();
            loadPlugin();
        },
        getDeviceInfo: function () {
            $.ajax({
                url: "/ISAPI/System/DeviceInfo",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.deviceinfo;
                        $("#sbbh_sbxq").val(device.devid);
                        $("#sbxx_sbxq").text(device.model);
                        $("#software_version").text(device.swversion);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setDeviceInfo: function () {
            var param = new Object();
            var content = new Object();
            content.devid = $("#sbbh_sbxq").val();
            param.deviceinfo = content;
            var json = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/System/DeviceInfo",
                type: "PUT",
                data: json,
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").css("display", "inline-block");
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getGPSStatus: function () {
            $.ajax({
                url: "/ISAPI/System/GPS/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.gpsStatus;
                        $("#gpsStatus").text(device.longitude + "," + device.latitude);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getSystemStatus: function () {
            $.ajax({
                url: "/ISAPI/System/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.sysStatus;
                        var time = device.uptime;
                        var str = "";
                        if (time <= 60) {
                            str = time + "秒";
                        } else if (time > 60 && time < 3600) {
                            str = Math.floor(time / 60) + "分" + (time - Math.floor(time / 60) * 60) + "秒";
                        } else {
                            var h = Math.floor(time / 3600);
                            var m = Math.floor((time - h * 3600) / 60);
                            str = h + "时" + m + "分" + (time - h * 3600 - m * 60) + "秒";
                        }
                        $("#runtime_sbxq").text(str);
                        $("#CPUtemperature").text(device.cpustat.temperature);
                        $("#memoryTemperature").text(device.memstat.temperature);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getNetworkStatus: function () {
            $.ajax({
                url: "/ISAPI/System/Network/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.networkStatus.wifiStatus.enable;
                        if (device) {
                            $(".sbxq-wifi-offline").hide();
                            $(".sbxq-wifi-online").show();
                        } else {
                            $(".sbxq-wifi-online").hide();
                            $(".sbxq-wifi-offline").show();
                        }
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getStoreStatus: function () {
            $.ajax({
                url: "/ISAPI/System/Store/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.storeStatus;
                        var total = device.total.substring(0, device.total.length - 2);
                        var free = device.freerate;
                        var use = parseFloat(total - (total * free / 100)).toFixed(4);

                        $(".cinpan-percentage").css("width", parseFloat(use * 120 / total).toFixed(4) + 'px');
                        $("#totalStorage").text(total + "GB");
                        $("#usedStorage").text(use);
                        $("#leaveStorage").text(parseFloat(free * total / 100).toFixed(4));
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getVideoInStatus: function () {
            $.ajax({
                url: "/ISAPI/VideoIn/Status",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var streamStat = data.videoinStatus[0].streamStat;
                        if (streamStat == 1) {
                            $(".sbxq-sp-offline").hide();
                            $(".sbxq-sp-online").show();
                        } else {
                            $(".sbxq-sp-online").hide();
                            $(".sbxq-sp-offline").show();
                        }
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveData: function () {

        }
    }
}();

//视频配置模块
var sppz = function () {
    function getSupportManufacturer(obj, code) {
        var str = '<select name="" id="" class="select-default sppz-shexiangji">';
        for (var i = 0; i < obj.length; i++) {
            if (code == i) {
                str += '<option value="' + obj[i].mancode + '" selected>' + obj[i].cnname + '</option>';
            } else {
                str += '<option value="' + obj[i].mancode + '">' + obj[i].cnname + '</option>';
            }
        }
        str += ' </select>';
        return str;
    };

    function getStreamtype(code) {
        var str = '<select name="" id="" class="select-default sppz-liumeiti">';
        if (code == 0) {
            str += '<option value="0" selected>主码流</option><option value="1">子码流</option>';
        } else {
            str += '<option value="0">主码流</option><option value="1" selected>子码流</option>';
        }

        str += ' </select>';
        return str;
    };

    return {
        addEvent: function () {
            child.addEvent();
            sppz.init();

            $(".a-offon").click(function () {
                $(this).toggleClass("a-offon-click");
            });

            $(".save-btn").click(function () {
                sppz.saveData();
            });

            $(".addChannel").click(function () {
                var max = $('#maxChannel').val();
                var len = $('.list-table tbody').find("tr").length;
                if (len >= 1) {
                    if (len == max) {
                        return;
                    } else {
                        var copy = $('.list-table').find('tbody tr')[0]; //通道不会为0，获取第一行默认数据。  
                        var td4 = $($(copy).find("td")[4]).html(); //厂商
                        var td5 = $($(copy).find("td")[5]).html();
                        var newLine = '<tr><td>CH' + (len + 1) + '</td>' +
                            '<td><input class="input-default" type="text" value=""></td>' +
                            '<td><input class="input-default" type="text" value=""></td>' +
                            '<td><input class="input-default" type="password" value=""></td>' +
                            '<td>' + td4 + '</td><td>' + td5 + '</td><td><a class="icon-cz icon-qy turnon" title="启用"></a><a class="icon-cz icon-kq turnoff" title="停止"></a>' +
                            '<a class="icon-cz icon-sc delChannel" title="删除"></a></td></tr>';
                        $('.list-table tbody').append(newLine);
                        if (max == len + 1) {
                            $('.addChannel').removeClass('addChannel-btn').addClass('disableChannel-btn');
                        }
                    }
                } else {

                }
            });

            $(".list-table tbody").on('click', '.delChannel', function () {
                var max = $('#maxChannel').val();
                var len = $('.list-table tbody').find("tr").length;
                if (len <= max && len > 1) {
                    $(this).parent().parent().remove();
                    $('.addChannel').removeClass('disableChannel-btn').addClass('addChannel-btn');
                }
            });

            $(".list-table tbody").on('click', '.icon-qy', function () {
                $(this).removeClass('icon-qy').addClass('icon-jy');
                $(this).next().removeClass('icon-kq').addClass('icon-tz');
            });
            $(".list-table tbody").on('click', '.icon-tz', function () {
                $(this).removeClass('icon-tz ').addClass('icon-kq');
                $(this).prev().removeClass('icon-jy').addClass('icon-qy');
            });
        },
        init: function () {
            $.ajax({
                url: "/ISAPI/VideoIn/Capabilities",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var re = new Array();
                        re = data.vedioinCap;
                        var maxCha = re.maxChannelNum; //支持的最大通道数 
                        var supportManufacturer = re.supportManufacturer; //支持的球机厂家列表  
                        // var tableTitle = '<tr><th width = "8%"> 通道 </th> <th width = "15%"> 摄像机IP </th> <th width = "15%"> 摄像机用户名 </th> <th width = "12%"> 摄像机密码 </th> ' + '<th width = "18%"> 摄像机厂家 </th> <th width = "17%"> 流媒体 </th> <th width = "15%"> 操作 </th> </tr>';
                        var str = '';
                        var source = sppz.getSource();
                        for (var i = 0; i < source.length; i++) {
                            var manufacturer = getSupportManufacturer(supportManufacturer, source[i].videosource.mancode);
                            var streamtype = getStreamtype(source[i].videosource.streamtype);
                            var enable = source[i].videosource.enable;
                            str += '<tr><td>' + source[i].videosource.chnname + '</td>' +
                                '<td><input type="text" class="input-default" value="' + source[i].videosource.cameraip + '"></td>' +
                                '<td><input type="text" class="input-default" value="' + source[i].videosource.username + '"></td>' +
                                '<td><input type="password" class="input-default" value="' + source[i].videosource.password + '"></td>' +
                                '<td>' + manufacturer + '</td>' +
                                '<td>' + streamtype + '</td>';
                            if (enable) {
                                str += '<td><a class="icon-cz icon-jy turnon" title="启用"></a><a class="icon-cz icon-tz turnoff" title="停止"></a>' +
                                    '<a class="icon-cz icon-sc delChannel" title="删除"></a></td></tr>';
                            } else {
                                str += '<td><a class="icon-cz icon-qy turnon" title="启用"></a><a class="icon-cz icon-kq turnoff" title="停止"></a>' +
                                    '<a class="icon-cz icon-sc delChannel" title="删除"></a></td></tr>';
                            };
                        }
                        $('#maxChannel').val(maxCha);
                        $('.list-table tbody').empty().append(str);
                        if (maxCha == source.length) {
                            $('.addChannel').removeClass('addChannel-btn').addClass('disableChannel-btn');
                        }
                        sppz.getOutSwitch();
                        sppz.getOutStategy();
                        sppz.getOutRtmpSvr();
                    } else {
                        alert("获取信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getSource: function () {
            var obj = {};
            $.ajax({
                url: "/ISAPI/VideoIn/Source",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        obj = data.sourcelist;
                    } else {
                        alert("获取信息失败!");
                        return;
                    }
                }
            });
            return obj;
        },
        getOutSwitch: function () {
            $.ajax({
                url: "/ISAPI/VideoOut/Switch?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.switch;
                        if (device) {
                            $(".a-offon").addClass("a-offon-click");
                        } else {
                            $(".a-offon").removeClass("a-offon-click");
                        }
                    } else {
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setOutSwitch: function () {
            var switchs = false;
            if ($(".a-offon").hasClass("a-offon-click")) {
                switchs = true;
            }
            var param = {
                "switch": switchs
            };
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/VideoOut/Switch?chnid=1",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        sppz.setOutStategy();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getOutStategy: function () {
            $.ajax({
                url: "/ISAPI/VideoOut/Strategy",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var strategy = data.videooutStrategy;
                        $("#spsc_protocol").val(strategy.protocol);
                    } else {
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setOutStategy: function () {
            var param = {};
            var con = {};
            var protocol = parseInt($("#spsc_protocol").val());
            if (isNaN(protocol)) fileformat = 0;
            con.protocol = protocol;
            param.videooutStrategy = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/VideoOut/Strategy",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        sppz.setOutRtmpSvr();
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        getOutRtmpSvr: function () {
            $.ajax({
                url: "/ISAPI/VideoOut/RtmpServer?chnid=1",
                type: "GET",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        var device = data.rtmpserver;                        
                        $("#spsc_rtmpserver").val(device.addr);
                    } else {
                        alert("查询信息失败!");
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        setOutRtmpSvr: function () {
            var param = {};
            var con = {};
            var rtmpaddr = $("#spsc_rtmpserver").val();
            con.addr = rtmpaddr;
            param.rtmpserver = con;
            param = JSON.stringify(param);
            $.ajax({
                url: "/ISAPI/VideoOut/RtmpServer?chnid=1",
                type: "PUT",
                data: param,
                datatype: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        $(".save-successful").css("display", "inline-block");
                    } else {
                        alert(data.result.description);
                        return;
                    }
                },
                error: function () {
                    child.logout();
                }
            });
        },
        saveData: function () {
            var list = $('.list-table tbody').find("tr");
            var params = new Object();
            var totalArray = new Array();
            var len = list.length;
            for (var i = 0; i < len; i++) {
                var obj = new Object();
                obj.chnid = i + 1;
                var videoSource = new Object();
                videoSource.chnname = $(list[i]).find('td:eq(0) input').val();
                videoSource.cameraip = $(list[i]).find('td:eq(1) input').val();
                videoSource.username = $(list[i]).find('td:eq(2) input').val();
                videoSource.password = $(list[i]).find('td:eq(3) input').val();
                videoSource.mancode = parseInt($(list[i]).find('td:eq(4) select').val());
                videoSource.streamtype = parseInt($(list[i]).find('td:eq(5) select').val());
                videoSource.enable = $(list[i]).find('td:eq(6) a:eq(0)').hasClass("icon-qy") == true ? false : true;
                obj.videosource = videoSource;
                totalArray.push(obj);
            }
            params.sourcelist = totalArray;
            var jsonAuth = JSON.stringify(params);
            $.ajax({
                url: "/ISAPI/VideoIn/Source",
                data: jsonAuth,
                type: "PUT",
                dataType: "json",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        //$(".save-successful").css("display", "inline-block");
                        sppz.setOutSwitch();
                    } else {
                        alert("保存信息失败!");
                        return;
                    }
                }
            });

        }
    }
}();

//查询模块
var qzsj = function () {
    function bindQZSJDialog(obj) {
        $(".yulan-tab li").removeClass("active");
        $($(".yulan-tab li")[0]).addClass("active");
        $("#tupian").show();
        $("#shipin").hide();
        var len = obj.attr("data-img");
        var deo = obj.attr("data-video");
        var imgs = [];
        $(".imgs").removeClass("cur");
        $(".tupian-side-ul li").removeClass("cur");
        $($(".tupian-main").find("img")[0]).addClass("cur");
        $($(".tupian-side-ul").find("li")[0]).addClass("cur");
        if (len.indexOf("##") > -1) {
            imgs = len.split("##");
        } else {
            imgs[0] = len;
        }
        $(".tupian-side-ul").find("li img").each(function (index) {
            if (imgs[index]) {
                $(this).attr("src", imgs[index]);
            } else {
                $(this).attr("src", "images/preview-" + (index + 1) + ".gif");
            }
        });
        $(".tupian-main").find("img").each(function (index) {
            if (imgs[index]) {
                $(this).attr("src", imgs[index]);
            } else {
                $(this).attr("src", "images/picture-" + (index + 1) + ".png");
            }
        });
        $('#video').attr("src", deo);
        $("#previewImg").show();
    }

    function setPlateColor(num) {
        num = parseInt(num);
        var str = '';
        switch (num) {
            case 4:
                str = "黑色"
                break;
            case 3:
                str = "蓝色"
                break;
            case 2:
                str = "黄色"
                break;
            case 1:
                str = "白色"
                break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                str = "新能源"
                break;
            default:
                str = "其它"
                break;
        }
        return str;
    };

    function setUrls(obj) {
        var str = "";
        for (var i = 0; i < obj.length; i++) {
            if ((i + 1) == obj.length) {
                str += obj[i].url;
            } else {
                str += obj[i].url + "##";
            }

        }
        return str;
    };

    return {
        initTime() {
    var defaultTime = child.initDefaultTime();
    var defaultStartTime = defaultTime + "00:00:00";
    var defaultEndTime = defaultTime + "23:59:59";
    $('#startTime_QZSJ').val(defaultStartTime);
    $('#endTime_QZSJ').val(defaultEndTime);
},
    addEvent: function () {
        child.addEvent();
        qzsj.initTime();
        $(".tupian-side-ul li").click(function () {
            $(this).addClass("cur").siblings().removeClass("cur");
            $(".imgs").removeClass("cur").eq($(this).index()).addClass("cur");
        });
        $("#fullScreenImg").click(function () {
            $("#fullScreenImg").hide();
        });
        $(".fullscreen").click(function () {
            var cUrl = $(".tupian-main").find(".cur").attr("src");
            $("#fullScreenImg img").attr("src", cUrl);
            $("#fullScreenImg").show();
            $("#fullScreenImg").addClass("full");
        });
        $(".closescreen").click(function () {
            $("#fullScreenImg").hide();
        });
        $(".yulan-tab li a").click(function () {
            $(".yulan-tab li").removeClass("active");
            $(this).parent().addClass("active");
            if ($(this).attr("data-window") == 1) {
                $('#tupian').show();
                $('#shipin').hide();
            } else {
                $('#tupian').hide();
                $('#shipin').show();
            }
        });

        $(".previousOne").click(function () { //上一个 
            if ($(".previewThisOne").prev()[0]) {
                var that = $(".previewThisOne");
                var next = that.removeClass("previewThisOne").prev();
                next.addClass('previewThisOne');
                bindQZSJDialog(next.find(".qzcxPreview"));

            } else {
                alert("已经是第一条数据!");
            }

        });

        $(".nextOne").click(function () { //下一个   
            if ($(".previewThisOne").next()[0]) {
                var that = $(".previewThisOne");
                var next = that.removeClass("previewThisOne").next();
                next.addClass('previewThisOne');
                bindQZSJDialog(next.find(".qzcxPreview"));

            } else {
                alert("已经是最后一条数据!");
            }

        });

        $(".popup-close").bind("click", function () {
            $('.qzcxPreview').parent().parent().removeClass("previewThisOne");
            $("#previewImg").hide();
        });
        $('#query_QZSJ').bind("click", function () {
            qzsj.init(1);
        });
        $('.qzsjTable').on('click', '.qzcxPreview', function () {
            $('.qzcxPreview').parent().parent().removeClass("previewThisOne");
            $(this).parent().parent().addClass("previewThisOne");
            bindQZSJDialog($(this));
        });

        $('.qzsjTable').on('click', '.qzcxDelete', function () { //删除
            var rowid = $(this).attr('data-rowid');
            $.ajax({
                url: "/ISAPI/Analysis/Results/Operate?rowid=" + rowid,
                type: "DELETE",
                cache: false,
                async: false,
                headers: {
                    "Authorization": "Basic " + localStorage.authorization
                },
                success: function (data) {
                    var code = data.result.code;
                    if (code == 0) {
                        qzsj.init(1);
                    } else {
                        alert("删除失败!");
                        return;
                    }
                }
            });
        });

        $('.searchPage').click(function () {
            var num = parseInt($('.pageNum').val());
            var currentPage = parseInt($('#pageno_QZSJ').html());
            var totalPage = parseInt($('#pagecnt_QZSJ').html());
            if (isNaN(num) || num == currentPage || num > totalPage || num < 1 || num == '' || num == null) return;
            qzsj.init(2);
        });
        $(".firstPage").click(function () {
            var currentPage = parseInt($('#pageno_QZSJ').html());
            if (currentPage == 1) return;
            $('#pageno_QZSJ').html('1');
            qzsj.init(1);
        });

        $(".lastPage").click(function () {
            var currentPage = parseInt($('#pageno_QZSJ').html());
            var totalPage = parseInt($('#pagecnt_QZSJ').html());
            if (currentPage == totalPage) return;
            $('#pageno_QZSJ').html($('#pagecnt_QZSJ').html());
            qzsj.init(1);
        });

        $(".prevPage").click(function () {
            var currentPage = parseInt($('#pageno_QZSJ').html());
            var totalPage = parseInt($('#pagecnt_QZSJ').html());
            if (currentPage <= totalPage && currentPage > 1) {
                $('#pageno_QZSJ').html(currentPage - 1);
                qzsj.init(1);
            }
        });
        $(".nextPage").click(function () {
            var currentPage = parseInt($('#pageno_QZSJ').html());
            var totalPage = parseInt($('#pagecnt_QZSJ').html());
            if (currentPage < totalPage) {
                $('#pageno_QZSJ').html(currentPage + 1);
                qzsj.init(1);
            }
        });

    },
init: function (num) {
    var params = new Object();
    var con = new Object();
    con.chnid = 1;
    con.start = $('#startTime_QZSJ').val();
    con.end = $('#endTime_QZSJ').val();
    con.plate = $('#cphm_QZSJ').val();
    con.datetype = parseInt($('#dataType_QZSJ').val());
    con.eventid = $('#dataType_QZSJ').val();
    con.platecolor = parseInt($('#color_QZSJ').val());
    con.eventid = 0;
    if (num == 1) {
        con.pageno = parseInt($('#pageno_QZSJ').html());
    } else {
        con.pageno = $('.pageNum').val() == null || $('.pageNum').val() == '' ? 1 : parseInt($('.pageNum').val());
    }

    con.pagesize = pageSize;
    params.analysisquery = con;
    var jsonAuth = JSON.stringify(params);

    $.ajax({
        url: "/ISAPI/Analysis/Results/Query",
        data: jsonAuth,
        type: "PUT",
        dataType: "json",
        cache: false,
        async: false,
        headers: {
            "Authorization": "Basic " + localStorage.authorization
        },
        success: function (data) {
            var code = data.result.code;
            if (code == 0) {
                var re = new Array();
                re = data.analysisresult;
                //  var pagecnt = re.pagecnt;
                var pageno = re.pageno;
                var reccnt = re.reccnt;
                var list = re.analylist;

                // var str1 = ' <tr> <th width=" 15 % ">序号</th><th width="25 % ">抓拍时间</th> <th width="15 % ">数据类型</th>' +
                //     ' <th width="15 % ">车牌号码</th> <th width="15 % ">车牌颜色</th> <th width=" 15 % ">预览</th> </tr> ';

                var str = '';
                if (list != null) {
                    for (var i = 0; i < list.length; i++) {
                        var type = list[i].datetype == 1 ? '违法' : '事件';
                        var imgs = setUrls(list[i].imgs);
                        var video = list[i].recs[0].url;
                        str += "<tr><td>" + list[i].rowid + '</td><td>' + list[i].time + '</td><td>' + type + '</td><td>' + list[i].plate + '</td><td>' +
                            setPlateColor(list[i].platecolor) + '</td><td><a class="icon-cz icon-yulan qzcxPreview" title="预览" data-img=' + imgs + ' data-video=' + video + '></a></td><td><a class="icon-cz icon-sc qzcxDelete" title="删除" data-rowid=' + list[i].rowid + ' ></a></td></tr>';
                    }
                }
                $('.qzsjTable tbody').empty().append(str);
                var pagecnt = Math.ceil(reccnt / pageSize);
                $('#pagecnt_QZSJ').text(pagecnt);
                $('#pageno_QZSJ').text(pageno);
                $('#reccnt_QZSJ').text(reccnt);
            } else {
                alert("获取用户信息失败!");
                return;
            }
        }
    });
}
}
}();

var record = function () {
    function bindVideoDialog(obj) {
        var deo = obj.attr("data-video");
        $('#video').attr("src", deo);
        $("#videoPreview").show();
    }
    return {
        initTime() {
    var defaultTime = child.initDefaultTime();
    var defaultStartTime = defaultTime + "00:00:00";
    var defaultEndTime = defaultTime + "23:59:59";
    $('#startTime_record').val(defaultStartTime);
    $('#endTime_record').val(defaultEndTime);
},
    addEvent: function () {
        child.addEvent();
        record.initTime();
        $('.recordTable').on('click', '.delRecord', function () {
            var did = parseInt($(this).attr('data-id'));
            if (confirm("确定要删除该条记录?")) {
                $.ajax({
                    url: "/ISAPI/Record/Operate?rowid=" + did,
                    type: "DELETE",
                    cache: false,
                    async: false,
                    headers: {
                        "Authorization": "Basic " + localStorage.authorization
                    },
                    success: function (data) {
                        var code = data.result.code;
                        if (code == 0) { //删除成功
                            record.init(1);
                        } else {
                            alert("获取用户信息失败!");
                            return;
                        }
                    }
                });
            }
        });


        $(".popup-close").click(function () {
            $('.qzcxPreview').parent().parent().removeClass("previewThisOne");
            $("#videoPreview").hide();
        });

        $(".previousOne").click(function () {
            if ($(".previewThisOne").prev()[0]) {
                var that = $(".previewThisOne");
                var next = that.removeClass("previewThisOne").prev();
                next.addClass('previewThisOne');
                bindVideoDialog(next.find(".qzcxPreview"));
            } else {
                alert("已经是第一条数据!");
            }
        });

        $(".nextOne").click(function () {
            if ($(".previewThisOne").next()[0]) {
                var that = $(".previewThisOne");
                var next = that.removeClass("previewThisOne").next();
                next.addClass('previewThisOne');
                bindVideoDialog(next.find(".qzcxPreview"));
            } else {
                alert("已经是最后一条数据!");
            }
        });

        $('#query_record').bind("click", function () {
            record.init(1);
        });

        $('.searchPage').click(function () {
            var num = parseInt($('.pageNum').val());
            var currentPage = parseInt($('#pageno_record').html());
            var totalPage = parseInt($('#pagecnt_record').html());
            if (isNaN(num) || num == currentPage || num > totalPage || num < 1 || num == '' || num == null) return;
            record.init(2);
        });
        $(".firstPage").click(function () {
            var currentPage = parseInt($('#pageno_record').html());
            if (currentPage == 1) return;
            $('#pageno_record').html('1');
            record.init(1);
        });

        $(".lastPage").click(function () {
            var currentPage = parseInt($('#pageno_record').html());
            var totalPage = parseInt($('#pagecnt_record').html());
            if (currentPage == totalPage) return;
            $('#pageno_record').html($('#pagecnt_record').html());
            record.init(1);
        });

        $(".prevPage").click(function () {
            var currentPage = parseInt($('#pageno_record').html());
            var totalPage = parseInt($('#pagecnt_record').html());
            if (currentPage <= totalPage && currentPage > 1) {
                $('#pageno_record').html(currentPage - 1);
                record.init(1);
            }
        });

        $(".nextPage").click(function () {
            var currentPage = parseInt($('#pageno_record').html());
            var totalPage = parseInt($('#pagecnt_record').html());
            if (currentPage < totalPage) {
                $('#pageno_record').html(currentPage + 1);
                record.init(1);
            }
        });

        $(".recordTable").on('click', '.recordVideo', function () {
            var url = $(this).attr("data-video");
            $("#video").attr("src", url);
            $('.recordVideo').parent().parent().removeClass("previewThisOne");
            $(this).parent().parent().addClass("previewThisOne");
            bindVideoDialog($(this));
        });

    },
init: function (num) {
    var params = new Object();
    var con = new Object();
    con.start = $('#startTime_record').val();
    con.end = $('#endTime_record').val();
    con.type = parseInt($('#type_record').val());
    con.chnid = 1;
    if (num == 1) {
        con.pageno = parseInt($('#pageno_record').html());
    } else {
        con.pageno = $('.pageNum').val() == null || $('.pageNum').val() == '' ? 1 : parseInt($('.pageNum').val());
    }
    con.pagesize = pageSize;
    params.recquery = con;
    var jsonAuth = JSON.stringify(params);
    $.ajax({
        url: "/ISAPI/Record/Query",
        data: jsonAuth,
        type: "PUT",
        dataType: "json",
        cache: false,
        async: false,
        headers: {
            "Authorization": "Basic " + localStorage.authorization
        },
        success: function (data) {
            var code = data.result.code;
            if (code == 0) {
                var re = new Array();
                re = data.recresult;
                // var pagecnt = re.pagecnt;
                var pageno = re.pageno;
                var reccnt = re.reccnt;
                var list = re.filelist;

                var str1 = '<tr> <th width = "15%"> 序号</th> <th width = "20%"> 录像开始时间 </th> <th width = "20%"> 录像结束时间 </th> <th width = "15%"> 录像类型 </th>' + ' <th width = "15%"> 删除 </th> <th width = "15%"> 预览 </th></tr>';

                var str = '';
                if (list != null) {
                    for (var i = 0; i < list.length; i++) {
                        str += "<tr><td>" + list[i].rowid + '</td><td>' + list[i].start + '</td><td>' + list[i].end + '</td><td>常规</td>' +
                            '<td><a class="icon-cz icon-shanchu delRecord" data-id="' + list[i].rowid + '" title="删除"></a></td>' +
                            '<td><a class="icon-cz icon-yulan recordVideo" data-video=' + list[i].path + ' title="预览"></a></td></tr>';
                    }
                }

                $('.recordTable').empty().append(str1 + str);
                var pagecnt = Math.ceil(reccnt / pageSize);
                $('#pagecnt_record').text(pagecnt);
                $('#pageno_record').text(pageno);
                $('#reccnt_record').text(reccnt);
            } else {
                alert("获取用户信息失败!");
                return;
            }
        }
    });
}
}
}();

var log = function () {
    function setEventType(num) {
        num = parseInt(num);
        var str = '';
        switch (num) {
            case 1:
                str = "用户管理"
                break;
            case 2:
                str = "系统维护"
                break;
            case 3:
                str = "系统配置"
                break;
            case 4:
                str = "视频输入"
                break;
            case 5:
                str = "视频录像"
                break;
            case 6:
                str = "智能算法"
                break;
            default:
                str = "其它"
                break;
        }
        return str;
    }

    return {
        initTime() {
    var defaultTime = child.initDefaultTime();
    var defaultStartTime = defaultTime + "00:00:00";
    var defaultEndTime = defaultTime + "23:59:59";
    $('#startTime_log').val(defaultStartTime);
    $('#endTime_log').val(defaultEndTime);
},
    addEvent: function () {
        child.addEvent();
        log.initTime();
        $('#query_log').bind("click", function () {
            log.init(1);
        });

        $('.searchPage').click(function () {
            var num = parseInt($('.pageNum').val());
            var currentPage = parseInt($('#pageno_log').html());
            var totalPage = parseInt($('#pagecnt_log').html());
            if (isNaN(num) || num == currentPage || num > totalPage || num < 1 || num == '' || num == null) return;
            log.init(2);
        });
        $(".firstPage").click(function () {
            var currentPage = parseInt($('#pageno_log').html());
            if (currentPage == 1) return;
            $('#pageno_log').html('1');
            log.init(1);
        });

        $(".lastPage").click(function () {
            var currentPage = parseInt($('#pageno_log').html());
            var totalPage = parseInt($('#pagecnt_log').html());
            if (currentPage == totalPage) return;
            $('#pageno_log').html($('#pagecnt_log').html());
            log.init(1);
        });

        $(".prevPage").click(function () {
            var currentPage = parseInt($('#pageno_log').html());
            var totalPage = parseInt($('#pagecnt_log').html());
            if (currentPage <= totalPage && currentPage > 1) {
                $('#pageno_log').html(currentPage - 1);
                log.init(1);
            }
        });

        $(".nextPage").click(function () {
            var currentPage = parseInt($('#pageno_log').html());
            var totalPage = parseInt($('#pagecnt_log').html());
            if (currentPage < totalPage) {
                $('#pageno_log').html(currentPage + 1);
                log.init(1);
            }
        });

        $('#log_export').click(function () {
            log.export();
        });
    },
init: function (num) {
    var params = new Object();
    var con = new Object();
    con.start = $('#startTime_log').val();
    con.end = $('#endTime_log').val();
    con.type = parseInt($('#type_log').val());
    con.like = '';
    if (num == 1) {
        con.pageno = parseInt($('#pageno_log').html());
    } else {
        con.pageno = $('.pageNum').val() == null || $('.pageNum').val() == '' ? 1 : parseInt($('.pageNum').val());
    }

    con.pagesize = pageSize;
    params.logquery = con;
    var jsonAuth = JSON.stringify(params);

    $.ajax({
        url: "/ISAPI/Log/Query",
        data: jsonAuth,
        type: "PUT",
        dataType: "json",
        cache: false,
        async: false,
        headers: {
            "Authorization": "Basic " + localStorage.authorization
        },
        success: function (data) {
            var code = data.result.code;
            if (code == 0) {
                var re = new Array();
                re = data.logresult;
                //var pagecnt = re.pagecnt;
                var pageno = re.pageno;
                var reccnt = re.reccnt;
                var list = re.loglist;

                var str1 = '<tr><th width = "10%"> 序号 </th><th width = "20%"> 上报时间 </th><th width = "15%"> 类型 </th> <th width = "15%"> 用户 </th>' + ' <th width = "20%">远程操作IP </th><th width="20%">内容</th></tr>';

                var str = '';
                if (list != null) {
                    for (var i = 0; i < list.length; i++) {
                        var type = setEventType(list[i].type);

                        str += "<tr><td>" + list[i].rowid + '</td><td>' + list[i].time + '</td><td>' + type + '</td><td>' + list[i].user + '</td><td>' +
                            list[i].addr + '</td><td>' +
                            list[i].message + '</td></tr>';
                    }
                }
                $('.logTable').empty().append(str1 + str);
                var pagecnt = Math.ceil(reccnt / pageSize);
                $('#pagecnt_log').text(pagecnt);
                $('#pageno_log').text(pageno);
                $('#reccnt_log').text(reccnt);
            } else {
                alert("获取用户信息失败!");
                return;
            }
        }
    });
},
            export: function () {
                var params = new Object();
                var con = new Object();
                con.start = $('#startTime_log').val();
                con.end = $('#endTime_log').val();
                con.type = parseInt($('#type_log').val());
                con.like = '';
                params.logquery = con;
                var jsonAuth = JSON.stringify(params);
                $.ajax({
                    url: "/ISAPI/Log/Export",
                    data: jsonAuth,
                    type: "PUT",
                    dataType: "json",
                    cache: false,
                    async: false,
                    headers: {
                        "Authorization": "Basic " + localStorage.authorization
                    },
                    success: function (data) {
                        var code = data.result.code;
                        if (code == 0) {
                            var url = data.export.filename;
                            window.open(url, '_blank');
                        } else {
                            alert("导出信息失败!");
                            return;
                        }
                    }
                });
            }
}

}();

function Base64() {
    // private property
    _keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    // public method for encoding
    this.encode = function (input) {
        var output = "";
        var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
        var i = 0;
        input = _utf8_encode(input);
        while (i < input.length) {
            chr1 = input.charCodeAt(i++);
            chr2 = input.charCodeAt(i++);
            chr3 = input.charCodeAt(i++);
            enc1 = chr1 >> 2;
            enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
            enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
            enc4 = chr3 & 63;
            if (isNaN(chr2)) {
                enc3 = enc4 = 64;
            } else if (isNaN(chr3)) {
                enc4 = 64;
            }
            output = output +
                _keyStr.charAt(enc1) + _keyStr.charAt(enc2) +
                _keyStr.charAt(enc3) + _keyStr.charAt(enc4);
        }
        return output;
    }
    // public method for decoding
    this.decode = function (input) {
        var output = "";
        var chr1, chr2, chr3;
        var enc1, enc2, enc3, enc4;
        var i = 0;
        input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
        while (i < input.length) {
            enc1 = _keyStr.indexOf(input.charAt(i++));
            enc2 = _keyStr.indexOf(input.charAt(i++));
            enc3 = _keyStr.indexOf(input.charAt(i++));
            enc4 = _keyStr.indexOf(input.charAt(i++));
            chr1 = (enc1 << 2) | (enc2 >> 4);
            chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
            chr3 = ((enc3 & 3) << 6) | enc4;
            output = output + String.fromCharCode(chr1);
            if (enc3 != 64) {
                output = output + String.fromCharCode(chr2);
            }
            if (enc4 != 64) {
                output = output + String.fromCharCode(chr3);
            }
        }
        output = _utf8_decode(output);
        return output;
    }
    // private method for UTF-8 encoding
    _utf8_encode = function (string) {
        string = string.replace(/\r\n/g, "\n");
        var utftext = "";
        for (var n = 0; n < string.length; n++) {
            var c = string.charCodeAt(n);
            if (c < 128) {
                utftext += String.fromCharCode(c);
            } else if ((c > 127) && (c < 2048)) {
                utftext += String.fromCharCode((c >> 6) | 192);
                utftext += String.fromCharCode((c & 63) | 128);
            } else {
                utftext += String.fromCharCode((c >> 12) | 224);
                utftext += String.fromCharCode(((c >> 6) & 63) | 128);
                utftext += String.fromCharCode((c & 63) | 128);
            }
        }
        return utftext;
    }
    // private method for UTF-8 decoding
    _utf8_decode = function (utftext) {
        var string = "";
        var i = 0;
        var c = c1 = c2 = 0;
        while (i < utftext.length) {
            c = utftext.charCodeAt(i);
            if (c < 128) {
                string += String.fromCharCode(c);
                i++;
            } else if ((c > 191) && (c < 224)) {
                c2 = utftext.charCodeAt(i + 1);
                string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
                i += 2;
            } else {
                c2 = utftext.charCodeAt(i + 1);
                c3 = utftext.charCodeAt(i + 2);
                string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
                i += 3;
            }
        }
        return string;
    }
}
