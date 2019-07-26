# mood_lamp2
This is a test.
Finally getting around to a new iteration of my mood lamp project, this time in (not in) CircuitPython!

So, CircuitPython didn't work out. I suppose it's possible I was doing something wrong, but for me at least it was such a dog performance-wise I couldn't get anywhere near a decent framerate out of these neopixels. I upgraded to faster boards a couple times (with the last being an M4) and it really just sucked. Even after I followed the optimization advice I found and basically de-Python-fied my code so completely it almost just looked like C.

So then I figured what the heck, let's just port it to Arduino. It's practically C already. And lo and behold, even on my lowly Feather HUZZAH board my frame rates went through the roof. Maybe CircuitPython has a use, but IMO neopixels ain't it.

Next up I made the hard decision to ditch Adafruit's Neopixel library and use FastLED instead. It's faster, but that's not really why I switched. I switched because it's got a lot of great, useful math functions built in. And it's faster. And after the whole CircuitPython experience, I kind of just want Adafruit out of my project entirely.

So it's still a work in progress, but the basics are there. Enjoy!
