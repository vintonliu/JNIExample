package org.wooyd.android.JNIExample;

/**
 * Created by vinton on 2016/12/08,0008.
 */

public class VideoPresetAdapter {
    public VideoPresetLowCpu low;
    public VideoPresetMediumCpu medium;
    public VideoPresetHighCpu high;

    public class VideoPresetLowCpu {
        public int complexity_w240;
        public int complexity_w360;
        public int complexity_w480;

        public int bitrate_w240;
        public int bitrate_w360;
        public int bitrate_w480;

        public int framerate_w240;
        public int framerate_w360;
        public int framerate_w480;

        public VideoPresetLowCpu() {
            complexity_w240 = complexity_w360 = complexity_w480 = 0;
            bitrate_w240 = bitrate_w360 = bitrate_w480 = 0;
            framerate_w240 = framerate_w360 = framerate_w480 = 0;
        }
    }

    public class VideoPresetMediumCpu {
        public int complexity_w240;
        public int complexity_w360;
        public int complexity_w480;

        public int bitrate_w240;
        public int bitrate_w360;
        public int bitrate_w480;

        public int framerate_w240;
        public int framerate_w360;
        public int framerate_w480;

        public VideoPresetMediumCpu() {
            complexity_w240 = complexity_w360 = complexity_w480 = 0;
            bitrate_w240 = bitrate_w360 = bitrate_w480 = 0;
            framerate_w240 = framerate_w360 = framerate_w480 = 0;
        }
    }

    public class VideoPresetHighCpu {
        public int complexity_w240;
        public int complexity_w360;
        public int complexity_w480;

        public int bitrate_w240;
        public int bitrate_w360;
        public int bitrate_w480;

        public int framerate_w240;
        public int framerate_w360;
        public int framerate_w480;

        public VideoPresetHighCpu() {
            complexity_w240 = complexity_w360 = complexity_w480 = 0;
            bitrate_w240 = bitrate_w360 = bitrate_w480 = 0;
            framerate_w240 = framerate_w360 = framerate_w480 = 0;
        }
    }
}
