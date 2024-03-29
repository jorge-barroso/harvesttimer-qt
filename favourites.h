#ifndef FAVOURITES_H
#define FAVOURITES_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <string>
#include <vector>
#include <QScrollArea>
#include "task.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Favourites;
}
QT_END_NAMESPACE

class Favourites : public QDialog
{
	Q_OBJECT

	public:
		explicit Favourites(const QDir& config_dir, QWidget* parent = nullptr);

		~Favourites() override;

		bool contains(const Task* task) const;

	signals:

		void add_task(Task* task);

		void task_added_to_favourites(const Task* task);

		void task_removed_from_favourites(const Task* task);

	public slots:

		void add_favourite_task(const Task* task);

		void remove_favourite_task(const Task* task);

        void logout_cleanup();

	private slots:

		void new_task_selected(const Task* task);

	private:
		Ui::Favourites* ui;
		const QString favourites_file_name = "favourites.dat";
		QFile favourites_file;
		std::vector<const Task*> tasks;

		void load_favourites();

		void save_favourites();

		void erase_task(const Task* task);
};

#endif // FAVOURITES_H
