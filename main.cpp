#include <QCoreApplication>
#include <QtNetwork>

QString GitHub_URL="https://api.github.com";

QString GitHub_User="YOUR USERNAME HERE";
QString GitHub_Token="YOUR TOKEN HERE";


QNetworkRequest GitHub_SetAuth(QString Url_Part, bool useBaseUrl=true) {
    QString concatenated = GitHub_User + ":" + GitHub_Token;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;

    QNetworkRequest request;
    QString url = GitHub_URL;
    url.append(Url_Part);

    if (useBaseUrl==false) {
        url=Url_Part;
    }

    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    return request;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QNetworkAccessManager nam;

    qDebug() << "User Auth";
    QNetworkRequest request = GitHub_SetAuth("/user/orgs");
    QNetworkReply *reply = nam.get(request);
    while(!reply->isFinished()) {
        qApp->processEvents();
        if (reply->error()) {
            qDebug() << reply->errorString();
            QByteArray response_data = reply->readAll();
            qDebug() << response_data;
            reply->deleteLater();
            return 0;
        }
    }
    QByteArray response_data = reply->readAll();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
    QJsonArray jsonArray = jsonResponse.array();
    int CountOrgs=jsonArray.size();
    for (int i = 0; i<CountOrgs;i++) {
        QJsonValueRef jsonValueRef = jsonArray[i];
        QJsonObject jsonValueObject = jsonValueRef.toObject();
        QString OrgaName = jsonValueObject["login"].toString();

        qDebug() << "Orga Repos";
        QNetworkRequest request = GitHub_SetAuth("/orgs/"+OrgaName+"/repos");
        QNetworkReply *reply = nam.get(request);
        while(!reply->isFinished()) {
            qApp->processEvents();
            if (reply->error()) {
                qDebug() << reply->errorString();
                QByteArray response_data = reply->readAll();
                qDebug() << response_data;
                reply->deleteLater();
                return 0;
            }
        }
        QByteArray response_data = reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
        QJsonArray jsonArray = jsonResponse.array();
        int CountRepos=jsonArray.size();
        for (int i = 0; i<CountRepos;i++) {
            QJsonValueRef jsonValueRef = jsonArray[i];
            QJsonObject jsonValueObject = jsonValueRef.toObject();
            QString RepoForkUrl = jsonValueObject["forks_url"].toString();
            //qDebug() << RepoForkUrl;

            qDebug() << "Fork Repo: " <<RepoForkUrl;
            QByteArray postData;
            QNetworkRequest request = GitHub_SetAuth(RepoForkUrl, false);
            QNetworkReply *reply = nam.post(request,postData);
            while(!reply->isFinished()) {
                qApp->processEvents();
                if (reply->error()) {
                    qDebug() << reply->errorString();
                    QByteArray response_data = reply->readAll();
                    qDebug() << response_data;
                    reply->deleteLater();
                    break;
                }
            }
            QByteArray response_data = reply->readAll();
            qDebug() << response_data;
        }

    }
    exit(0);
    return a.exec();
}
