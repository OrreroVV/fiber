import requests


headers = {
    "accept": "*/*",
    "accept-language": "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6",
    "content-type": "application/x-www-form-urlencoded",
    "priority": "u=1, i",
    "referer": "https://weibo.com/",
    "sec-ch-ua": "\"Microsoft Edge\";v=\"135\", \"Not-A.Brand\";v=\"8\", \"Chromium\";v=\"135\"",
    "sec-ch-ua-mobile": "?0",
    "sec-ch-ua-platform": "\"Windows\"",
    "sec-fetch-dest": "script",
    "sec-fetch-mode": "no-cors",
    "sec-fetch-site": "same-site",
    "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0",
    "x-requested-with": "XMLHttpRequest"
}
cookies = {
    "XSRF-TOKEN": "FM1sySAPPQrg9VmU4udCtKjL",
    "SCF": "AtVl3cO-0Tm_5-9CSJ6kGgLWRAkhis3gS9rZxJrA4ueEtznwC55kBhSVmX05jVfVOihRfkqG7-wxPU8AvzTSric.",
    "SUB": "_2A25FIfMqDeRhGeBL41oZ-SbJyDWIHXVmXwrirDV8PUNbmtAYLUzYkW9NRtIISJk1LPvMG5_Y8Vk_mvCcVPDgm9Fn",
    "SUBP": "0033WrSXqPxfM725Ws9jqgMF55529P9D9Wh4NbR-gyDZadvfFd9rV.vj5JpX5KzhUgL.Foqf1hnR1Knfe0.2dJLoI7UlMriLPNia",
    "ALF": "02_1749880954",
    "_s_tentry": "passport.weibo.com",
    "Apache": "7046494445488.163.1747288956764",
    "SINAGLOBAL": "7046494445488.163.1747288956764",
    "ULV": "1747288956791:1:1:1:7046494445488.163.1747288956764:",
    "WBPSESS": "KAFqrTKSOWv8fyTkr6cXT0mXsI_aMrKIMrOFeMY2sFDvmrVSR_iCutGPCGq6RKgPCBaCbTwbEyWMfZuZY3GNa_dtyPxxrsOP9OEkyap4RP0NUNAKjxXiKfXrLr8JTCud4b5xVPHccPLv4sk4ALTz7Q==",
    "wb_view_log_6588898539": "2560*10801",
    "PC_TOKEN": "6a6696c4c2",
}
url = "https://weibo.com/p/aj/general/button"
params = {
    "ajwvr": "6",
    "api": "http://i.huati.weibo.com/aj/super/checkin",
    "texta": "已签到",
    "textb": "已签到",
    "status": "1",
    "id": "1008082d890b7e5ae8b7248f8e6e9112f9ec15",
    "location": "page_100808_super_index",
    "timezone": "GMT 0800",
    "lang": "zh-cn",
    "plat": "Win32",
    "ua": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0",
    "screen": "2560*1080",
    "__rnd": "1747290881327"
}
response = requests.get(url, headers=headers, cookies=cookies, params=params)

print(response.text)
print(response)