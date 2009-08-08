#ifndef H_WEDIT
#define H_WEDIT

#include <QWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QPushButton>

class WEdit : public QWidget
{
	Q_OBJECT

public:
	WEdit(QWidget* parent = 0);
	void save(QString path);
	void load(QString path);

public slots:
	void render();
	void newDoc();
	void save();
	void saveAs();
	void load();

private:
	QString currentPath;
	QTextEdit* input;
	QTextBrowser* output;
	QPushButton* newButton;
	QPushButton* saveButton;
	QPushButton* saveAsButton;
	QPushButton* loadButton;
};

#endif // H_WEDIT
