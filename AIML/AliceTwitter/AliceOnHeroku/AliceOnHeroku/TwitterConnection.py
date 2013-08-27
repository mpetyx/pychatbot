__author__ = 'mpetyx'

from django.conf import settings
import tweepy

from .models import retrieve


class twitter():

    def __init__(self):

        #initializing variables
        consumer_key = settings.consumer_key
        consumer_secret = settings.consumer_secret
        key = settings.key
        secret = settings.secret


        auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
        auth.set_access_token(key, secret)
        self.api = tweepy.API(auth)

        self.auth = auth

    def postFromChat(self):

        message = xmpp.Message(self.request.POST)
        message.reply("let's try post this")
        self.api.update_status("My friend @%s just told me, %s"%(message.sender ,message.body))
        #time.sleep(3600)
        message.reply(" I just sent that to twitter!!")

        retrieve(self.auth).statistics()

    def reply(self):

        return 1

    def postAnything(self):

        return 1

    def postAndMentionSomeone(self):

        return 1

    def friends(self):

        self.friends = self.api.friends()
        self.followers = self.api.followers()