import QtQuick 2.0
import Sailfish.Silica 1.0

Page {

    property var settingsPage
    property var notification

    SilicaWebView {
        id: webView
        anchors.fill: parent
        url: evernoteSynchronizer.createAuthenticationUrl()
        onUrlChanged: authorize(url.toString())
    }

    function authorize(url) {
        if (url.indexOf('nnoauth?') !== -1 && url.indexOf('?oauth_token=') !== -1) {
            webView.stop();
            if (url.indexOf('&oauth_verifier=') !== -1) {
                evernoteSynchronizer.authenticate(url);
                evernoteSynchronizer.startSynchronization();
                settingsPage.setAuthorized(true);
                notification.publish();
            }
            pageStack.pop();
        }
    }
}
