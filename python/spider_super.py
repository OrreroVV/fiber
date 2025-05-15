import requests


headers = {
    "accept": "*/*",
    "accept-language": "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6",
    "referer": "https://weibo.com/p/1008082d890b7e5ae8b7248f8e6e9112f9ec15/super_index",
    "sec-ch-ua": "\"Microsoft Edge\";v=\"135\", \"Not-A.Brand\";v=\"8\", \"Chromium\";v=\"135\"",
    "sec-ch-ua-mobile": "?0",
    "sec-ch-ua-platform": "\"Windows\"",
    "sec-fetch-dest": "empty",
    "sec-fetch-mode": "cors",
    "sec-fetch-site": "same-origin",
    "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0",
    "content-type": "application/x-www-form-urlencoded",
    "priority": "u=1, i",
    "x-requested-with": "XMLHttpRequest"
}
cookies = {
    "SCF": "AtVl3cO-0Tm_5-9CSJ6kGgLWRAkhis3gS9rZxJrA4ueEtznwC55kBhSVmX05jVfVOihRfkqG7-wxPU8AvzTSric.",
    "SUB": "_2A25FIfMqDeRhGeBL41oZ-SbJyDWIHXVmXwrirDV8PUNbmtAYLUzYkW9NRtIISJk1LPvMG5_Y8Vk_mvCcVPDgm9Fn",
    "SUBP": "0033WrSXqPxfM725Ws9jqgMF55529P9D9Wh4NbR-gyDZadvfFd9rV.vj5JpX5KzhUgL.Foqf1hnR1Knfe0.2dJLoI7UlMriLPNia",
    "ALF": "02_1749880954",
    "_s_tentry": "passport.weibo.com",
    "Apache": "7046494445488.163.1747288956764",
    "SINAGLOBAL": "7046494445488.163.1747288956764",
    "ULV": "1747288956791:1:1:1:7046494445488.163.1747288956764:",
    "PC_TOKEN": "6a6696c4c2",
    "webim_unReadCount": "%7B%22time%22%3A1747289566097%2C%22dm_pub_total%22%3A0%2C%22chat_group_client%22%3A0%2C%22chat_group_notice%22%3A0%2C%22allcountNum%22%3A19%2C%22msgbox%22%3A0%7D",
    "WBPSESS": "KAFqrTKSOWv8fyTkr6cXT0mXsI_aMrKIMrOFeMY2sFDvmrVSR_iCutGPCGq6RKgPCBaCbTwbEyWMfZuZY3GNa_dtyPxxrsOP9OEkyap4RP0NUNAKjxXiKfXrLr8JTCud4b5xVPHccPLv4sk4ALTz7Q==",
    "wb_view_log_6588898539": "2560*10801"
}
url = "https://s.weibo.com/ajax/jsonp/gettopsug"
params = {
    "uid": "6588898539",
    "ref": "PC_topsug",
    "url": "https://weibo.com/p/1008082d890b7e5ae8b7248f8e6e9112f9ec15/super_index",
    "Mozilla": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0",
    "_cb": "STK_174728944137027"
}
response = requests.get(url, headers=headers, cookies=cookies, params=params)

print(response.text)
print(response)