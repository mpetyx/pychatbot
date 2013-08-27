__author__ = 'mpetyx'

from django.db import models


class TwitterInteraction(models.Model):
    user_name = models.TextField()
    post_id = models.TextField()
    created = models.DateTimeField(auto_now_add=True)
    updated = models.DateTimeField(auto_now=True)
    screen_name = models.TextField()
    text = models.TextField()
    #profile_url = models.StringProperty()
    #access_token = models.StringProperty()

def add_interaction(user_name, screen_name, text, post_id):

    interaction = TwitterInteraction(user_name= user_name, screen_name = screen_name, text = text, post_id = post_id)
    interaction.put()


    return 1


class retrieve:


    def __init__(self, handler=None):
        """
        I suppose i have a connection, otherwise i open one
        """
        if handler==None:
            self.handler = "initialize connection"

        else:
            self.handler = handler

        self.count = 5


    def RoundRobin(self):

            """
            Here we are going to decide about the statuses
            that we are going to track
            """
            return 1

    def statistics(self):
        #https://dev.twitter.com/doc/get/statuses/mentions

        auth = self.handler

        import tweepy

        api = tweepy.API(auth)

        #lolen = api.retweets( id, count=self.count)# ,since_id= self.last_twitter_id)
        lolen = api.retweets_of_me()
        statuses = []
        lolen = lolen + api.retweeted_to_me()
        for status in lolen:

            user = status.user
            name = user.screen_name
            text = status.text
            id = status.id
            full_name = user.name
            #http://apiwiki.twitter.com/w/page/22554664/Return-Values
            created_at = status.created_at
            statuses.append(self.myStatus(name,text,id,full_name))

        for mention in api.mentions(count=self.count):# ,since_id= self.last_twitter_id)
            user = mention.user
            name = user.screen_name
            text = mention.text
            id = mention.id
            full_name = user.name
            #http://apiwiki.twitter.com/w/page/22554664/Return-Values

            created_at = mention.created_at

            myMention = self.myStatus(name,text,id,full_name)

            if myMention in statuses:
                continue
            else:

                statuses.append(myMention)

        for status in statuses:
            add_interaction(user_name = status['full_name'], screen_name = status['name'], text = status['text'], post_id = status['id'])

    def create_friendship(self, friend):
        return self.handler.create_friendship(friend)

    def myStatus(self, name, text,id, full_name):
        id = str(id)

        return {'name':name, 'text':text, 'id':id,"full_name":full_name}
