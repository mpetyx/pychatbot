__author__ = 'mpetyx'


from django.contrib import admin
from models import *

class TwitterAdmin(admin.ModelAdmin):
    pass


admin.site.register(TwitterInteraction, TwitterAdmin)
