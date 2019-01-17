#ifndef SQL_H
#define SQL_H
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QtSql>
static bool CreatConnection()
{
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("music.db");
    if(!db.open())
    {
        return false;
    }
    QSqlQuery query;
    query.exec(QString("create table MUSIC (id int primary key,MusicName varchar,FileName varchar,biaoji int)"));
    query.exec(QString("create table VIDEO (id int primary key,VideoName varchar,FileName varchar,biaoji int)"));
    return true;
}
#endif // SQL_H
