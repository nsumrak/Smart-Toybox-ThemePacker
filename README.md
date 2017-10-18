# Smart Toybox Theme Packer App

Theme packer is helper application for [Smart Toybox project](https://github.com/nsumrak/Smart-Toybox-Embedded-TI). There you can find [demo theme.stb](https://github.com/nsumrak/Smart-Toybox-Embedded-TI/blob/master/deploy/uniflash_config_session/fileSystem/theme.stb) file to play with this application.

Kids get bored with same routine very quickly. So, we thought ways to make Smart Toybox exciting for a long time. That is how we've came to the concept of Themes.

Theme is a collection of sounds. These sounds are 16-bit mono [WavPack](http://wavpack.com) compressed samples, organized in “music” playlists which defines order in which sounds will be played, silence periods and repetitions. You can imagine sounds to be tones of some instruments, then these playlists would allow playing of music themes. Or if you imagine sounds to be words, playlists allow connecting them into sentences, saving very sparse space on embedded flash memory. Software is designed to play two independent music streams.

Each 'music' has 'type' property which defines mood. When Smart Toybox software needs to express its mood, it will play one of the music playlists of specific mood type either selecting it randomly or consecutively among all music playlists of specific type. Here are currently defined types:

0.  Played on game start
1.  Played when game is "won" (enough toys in the box)
2.  Played when game is "lost" (not so much toys in the box)
3.  This type is game background music
4.  Played when toybox asks for toys
5.  "Chewing" toys are being put in the box
6.  "Grand chew" – large toys are being but into the box
8.  Played when toys settle inside the box ("gulp")
9.  Played when large quantity of toys settles in the box
10. Complaining when toys are taken from the box during game
11. Box is being turned on the side during the game
12. Box stands correctly again during the game
13. Played to indicate good progress with toys collecting game
14. Played to indicate bad progress with toys collecting game
15. Box is being turned on the side (outside of the game)
16. GUARDIAN Box stands correctly again (outside of the game)

Finally, all these data is organized in one single “.STB” file. We’ve made this decision because flash memory on Launchpad board is only 1MB, including firmware and network configuration files, and implemented file system is highly inefficient. Our idea was that [mobile application](https://github.com/nsumrak/Smart-Toybox-MobileApp) could be used to easily create and share themes. But we needed theme files immediately, before we develop those functionalities, so we created helper application for Windows called "Smart Toybox Theme Packer". Unfortunately, even today this application is still the only way to create Smart Toybox theme.

## How to compile

We used Microsoft Visual Studio Community Edition 2015 to make and compile this application.
