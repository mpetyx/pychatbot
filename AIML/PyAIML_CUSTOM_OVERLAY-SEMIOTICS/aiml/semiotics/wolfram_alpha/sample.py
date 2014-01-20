__author__ = 'mpetyx'


import wolframalpha

app_id = "VJ5EPP-AYP7RWQQ5J"

client = wolframalpha.Client(app_id)


# res = client.query('temperature in Washington, DC on October 3, 2012')

res = client.query('who is michael jackson?')

for pod in res.pods:
    # do_something_with(pod)
    print pod.text

# print(next(res.results).text)

print res.results.text