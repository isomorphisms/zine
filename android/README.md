# First poem on the phone

Open Zine to read the first poem in `sequence.tsv`. Android supplies accessible,
selectable serif text, scrolling and a link to the cited edition. The bundled
text works offline. There is no timer, automatic advance, account or feed.

This first slice reads one poem, currently Frost's “Nothing Gold Can Stay”.
The other four poems remain bundled corpus; deliberate navigation is later work.
Original newline characters remain in the text; Android may additionally wrap a
long line to fit the screen. Font size follows Android's accessibility scaling.

The implementation uses the requested NativeActivity / C path, invoking Android
platform widgets through JNI. It contains no authored Java, Kotlin or DEX and
uses no Gradle. The application logic is deliberately limited to selecting the
first sequence entry and loading its separately stored metadata and text.

Build on a Linux host with Android SDK 36, build-tools 36.0.0 and NDK
27.2.12479018 using `make -f /absolute/path/to/zine/android/Makefile`.
The workflow checks out the existing public Wegert test key at its pinned
revision. Package identity is `org.walnutburgundy.zine`, version code 1.
The expected test certificate is
`de9b1d47c5a65e6d46a204b79dd9ee566b9d3c9832ba81ebc4213d3392e92ff9`.
This key is public test material, not a production signing identity.

The ARMv7 APK targets MIRO A1. The x86_64 APK exists for emulator checks only.
Physical acceptance remains pending until this exact APK is opened on the
phone: verify the complete poem, legible punctuation, scrolling, source link,
return from another app, and replacement installation without uninstalling.
