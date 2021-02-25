package com.joshuasterner.openclstats

class CLInfo {

    external fun logInfo()

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}