package com.countdown;

import android.content.Intent;
import android.net.Uri;
import androidx.core.content.FileProvider;
import org.qtproject.qt.android.QtNative;
import java.io.File;

public class Installer {
    public static void installApk(String apkPath) {
        if (QtNative.activity() == null) return;

        File apkFile = new File(apkPath);
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

        Uri apkUri = FileProvider.getUriForFile(
            QtNative.activity(),
            QtNative.activity().getPackageName() + ".fileprovider",
            apkFile
        );

        intent.setDataAndType(apkUri, "application/vnd.android.package-archive");
        QtNative.activity().startActivity(intent);
    }
}