var url = '/ISAPI/interface';

$(document).ready(function () {
    main.mainLoadURL('shishijiankong.html') + new Date().getTime();
    main.addEvent();
});

/**
 * 主页面功能
 * @returns {Boolean|Object} [[Description]]
 */
var main = function () {
    return {
        addEvent: function () {
            //主页面的页面切换
            $('.side-nav-box li a').bind('click', function () {
                var url = $(this).attr('data-url') + '.html?timestamp=' + new Date().getTime();
                $('.side-caidan-active').removeClass('side-caidan-active');
                $(this).parent().addClass('side-caidan-active');
                main.mainLoadURL(url);
            }); 

            //头部折叠按钮
            $(".ptop-list").click(function () {
                $(".side-nav-box").toggleClass("active");
                $(this).toggleClass("active");
            });

            //播放器操作
            $(".player-control-play").click(function () {
                $(this).toggleClass("active");
            });

            $(".player-control-pause").click(function () {
                $(this).toggleClass("active");
            });
        },
        mainLoadURL: function (url) { 
            $(".top-yh-top span").text(localStorage.unnn);
            $("#mainContent").empty();
            $("#mainContent").load(url);
        }
    };
}();
