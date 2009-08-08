#include "wedit.h"
#include "extlib/NME.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QShortcut>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

WEdit::WEdit(QWidget* parent) : QWidget(parent) {
	input = new QTextEdit();
	input->setAcceptRichText(false);

	output = new QTextBrowser();
	output->setOpenExternalLinks(true);
	output->setReadOnly(true);

	newButton = new QPushButton(tr("&New"));
	loadButton = new QPushButton(tr("&Load..."));
	saveButton = new QPushButton(tr("&Save"));
	saveAsButton = new QPushButton(tr("Save &As..."));

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(newButton);
	buttonLayout->addWidget(loadButton);
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(saveAsButton);

	/* TODO Should allow switching between horiz/vertical */
	/* TODO Should allow hiding the edit box */
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(input);
	layout->addWidget(output);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	setWindowTitle(tr("WEdit"));

	QStringList args = qApp->arguments();
	if(args.size() == 2) {
		load(args.at(1));
	}

	/* XXX Doing a complete re-render on every edit may not scale */
	connect(input, SIGNAL(textChanged()), this, SLOT(render()));
	connect(newButton, SIGNAL(clicked()), this, SLOT(newDoc()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
	connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));

	new QShortcut(*new QKeySequence("Ctrl+n"), this, SLOT(newDoc()), SLOT(newDoc()), Qt::ApplicationShortcut);
	new QShortcut(*new QKeySequence("Ctrl+s"), this, SLOT(save()), SLOT(save()), Qt::ApplicationShortcut);
	new QShortcut(*new QKeySequence("Ctrl+o"), this, SLOT(load()), SLOT(load()), Qt::ApplicationShortcut);

}

/* Slots */

void WEdit::render() {
	QByteArray inputBytes = input->toPlainText().toUtf8();
	NMEText input = inputBytes.data();
	NMEInt inputLength = inputBytes.count();
	if(inputLength > 0) {
		NMEInt size = inputBytes.count();
		NMEText buf = (NMEChar*)malloc(size);
		NMEText out;
		NMEInt outLength;
		NMEErr err;
		while(true) {
			err = NMEProcess(input, inputLength, buf, size,
			                 kNMEProcessOptDefault, "\n", &NMEOutputFormatHTML, 0,
			                 &out, &outLength, 0);
			if(err == kNMEErrNotEnoughMemory) {  // unlikely
				size *= 2;
				buf = (NMEChar*)realloc(buf, size);
			} else {
				break;
			}
		}
		if (err == kNMEErrOk) {
			output->setHtml(QString::fromUtf8(out));
		} else {
			qDebug() << err;
		}
		free(buf);
	} else {
			output->setHtml("");
	}
}

void WEdit::newDoc() {
	input->setPlainText("");
	render();
	currentPath = "";
}

void WEdit::save() {
	if(currentPath != "") {
		save(currentPath);
	} else {
		saveAs();
	}
}

void WEdit::saveAs() {
	QString documentsPath = QDir::homePath();
	if(QDir(QDir::homePath() + QDir::separator() + tr("Documents") + QDir::separator()).exists()) documentsPath = QDir::homePath() + QDir::separator() + tr("Documents") + QDir::separator();
	if(QDir(QDir::homePath() + QDir::separator() + tr("My Documents") + QDir::separator()).exists()) documentsPath = QDir::homePath() + QDir::separator() + tr("My Documents") + QDir::separator();
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), documentsPath, tr("All Files (*.*)"));
	save(fileName);
}

void WEdit::load() {
	QString documentsPath = QDir::homePath();
	if(QDir(QDir::homePath() + QDir::separator() + tr("Documents") + QDir::separator()).exists()) documentsPath = QDir::homePath() + QDir::separator() + tr("Documents") + QDir::separator();
	if(QDir(QDir::homePath() + QDir::separator() + tr("My Documents") + QDir::separator()).exists()) documentsPath = QDir::homePath() + QDir::separator() + tr("My Documents") + QDir::separator();
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load..."), documentsPath, tr("All Files (*.*)"));
	load(fileName);
}

/* Functions */

void WEdit::save(QString path) {
	if(path == "") return;
	qDebug() << "Saving to: " << path;

	QFile file(path);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "File could not be written";
	} else {
		QTextStream stream(&file);
		stream << input->toPlainText();
		file.close();
	}

	currentPath = path;
}

void WEdit::load(QString path) {
	if(path == "") return;
	qDebug() << "Loading from: " << path;

	input->setPlainText("");

	QFile file(path);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "File could not be read";
	} else {
		QTextStream stream(&file);
		input->setPlainText(stream.readAll());
		file.close();
	}

	render();

	currentPath = path;
}
