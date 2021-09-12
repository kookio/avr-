from os import startfile
from tkinter.constants import NONE
import requests
import json
from datetime import *
import time
import Get_data
class kakao_send():
    def tokens():                                                                                                           #KAKAO메세지를위한 토큰 발급 리프레쉬가 되지않았을때 토큰값을 발급해야함
        url = "https://kauth.kakao.com/oauth/token"

        data = {
            "grant_type" : "authorization_code",
            "client_id" : "82f6d607662b80e45a70472dfc754ba4",
            "redirect_uri" : "https://localhost.com",
            "code"         : "lo8uxUZU9aZ1tzgFTf2KqBn1g51cp5cHde8Nn141EoubqMNxLOE5zfSRQT-v1eYMZeAZoQopyV8AAAF7x8ka0w"
        }
        response = requests.post(url, data=data)

        tokens = response.json()

        with open("kakao_token.json", "w") as fp:
            json.dump(tokens, fp)

    def refreshToken():                                                                                                     #토큰값 리프레쉬
        with open("kakao_token.json","r") as fp:
            token = json.load(fp)
        REST_API_KEY = "82f6d607662b80e45a70472dfc754ba4"
        url = "https://kauth.kakao.com/oauth/token"

        data = {
            "grant_type": "refresh_token",                                                                                  # 얘는 단순 String임. "refresh_token"    
            "client_id":f"{REST_API_KEY}",
            "refresh_token": token['refresh_token']                                                                         # 여기가 위에서 얻은 refresh_token 값
        }    
    
        resp = requests.post(url , data=data)
        token['access_token'] = resp.json()['access_token']
        with open("kakao_token.json", "w") as fp:
            json.dump(token, fp)
        return(token)



    def kakao_text():                                                                                                       #KAKAO메세지를 보내는 코드
        list_time = []
        list_time.insert(0,time.strftime('%y-%m-%d %H:%M:%S'))
        get_error_code = Get_data.PLC_A()
        error_code = get_error_code.error_code()
        get_error_code_2 = Get_data.PLC_B()
        error_code_2 = get_error_code_2.error_code()
        print(error_code)
        print(error_code_2)
        with open("kakao_token.json","r") as fp:    
            tokens = json.load(fp)
        kcreds = {
            "access_token" : tokens.get('access_token')
        }
        kheaders = {
            "Authorization": "Bearer " + kcreds.get('access_token')
        }

        kakaotalk_template_url = "https://kapi.kakao.com/v2/api/talk/memo/default/send"

        # 날씨 상세 정보 URL
        weather_url = "https://search.naver.com/search.naver?sm=top_hty&fbm=0&ie=utf8&query=%EB%82%A0%EC%94%A8"
        
        text = f"""                                                                    
        {list_time[0]} \nError Code\nEC_Error {error_code[0]}\nMC_Error {error_code[1]}\n에러가 발생했습니다. 기동을 중지합니다.
        """
        template = {
        "object_type": "text",
        "text": text,
        "link":{"web_url" : weather_url}
                                                                                                                # "button_title": "날씨 상세보기"
        }                                                                                                             # 날씨 정보 만들기
        
                                                                                                                    
        payload = {
            "template_object" : json.dumps(template)
        }                                                                                                                   # JSON 형식 -> 문자열 변환 

                                                                                                                    
        res = requests.post(kakaotalk_template_url, data=payload, headers=kheaders)                                         # 카카오톡 보내기

        if res.json().get('result_code') == 0:
            print('메시지를 성공적보냈습니다.')
        else:
            print('메시지를 성공적으로 보내지 못했습니다. 오류메시지 : ' + str(res.json()))
def kakao_text_2():                                                                                                       #KAKAO메세지를 보내는 코드
        list_time = []
        list_time.insert(0,time.strftime('%y-%m-%d %H:%M:%S'))
        get_error_code_2 = Get_data.PLC_B()
        error_code_2 = get_error_code_2.error_code()
        print(error_code_2)
        with open("kakao_token.json","r") as fp:    
            tokens = json.load(fp)
        kcreds = {
            "access_token" : tokens.get('access_token')
        }
        kheaders = {
            "Authorization": "Bearer " + kcreds.get('access_token')
        }

        kakaotalk_template_url = "https://kapi.kakao.com/v2/api/talk/memo/default/send"

        # 날씨 상세 정보 URL
        weather_url = "https://search.naver.com/search.naver?sm=top_hty&fbm=0&ie=utf8&query=%EB%82%A0%EC%94%A8"
        
        text = f"""                                                                    
        {list_time[0]} \nError Code\nEC_Error {error_code_2[0]}\nMC_Error {error_code_2[1]}\n에러가 발생했습니다. 기동을 중지합니다.
        """
        template = {
        "object_type": "text",
        "text": text,
        "link":{"web_url" : weather_url}
                                                                                                                # "button_title": "날씨 상세보기"
        }                                                                                                             # 날씨 정보 만들기
        
                                                                                                                    
        payload = {
            "template_object" : json.dumps(template)
        }                                                                                                                   # JSON 형식 -> 문자열 변환 

                                                                                                                    
        res = requests.post(kakaotalk_template_url, data=payload, headers=kheaders)                                         # 카카오톡 보내기

        if res.json().get('result_code') == 0:
            print('메시지를 성공적보냈습니다.')
        else:
            print('메시지를 성공적으로 보내지 못했습니다. 오류메시지 : ' + str(res.json()))
