#include "db.h"

QQmlEngine *APP_DB::_engine = nullptr;

APP_DB::APP_DB(QQmlApplicationEngine *parent) : QObject(parent)
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    this->setParent(parent);

    // DEBUG STUFF
    //QString crypt_pw = QCryptographicHash::hash(QString("abcd").toLocal8Bit(),
    //        QCryptographicHash::Sha256).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    //qDebug() << "TEST PW: " << crypt_pw;

    //QQmlApplicationEngine *p = qobject_cast<QQmlApplicationEngine*>(this->parent());
    APP_DB::_engine = qobject_cast<QQmlApplicationEngine*>(this->parent());
    if (APP_DB::_engine == nullptr) {
        // Unable to get app engine
        qDebug() << "FATAL ERROR - APP_DB Unable to get the QML Application Engine.";
        QCoreApplication::quit();
        return;
    }

    // Expose the generic table model
    qmlRegisterType<GenericQueryModel>("com.openprisoneducation.ope", 1, 0, "GenericQueryModel");
    qmlRegisterType<GenericTableModel>("com.openprisoneducation.ope", 1, 0, "GenericTableModel");

    //p->rootContext()->setContextProperty("database", this);
}

/**
 * @brief DB::init_db - Connect to database and ensure that scheme is in place and/or migrated
 * @return true on success
 */
bool APP_DB::init_db(QString db_file)
{
    bool ret = false;

    _db.setHostName(db_file);
    _db.setDatabaseName(db_file);
    //_db.setPassword("TEST PW"); // TODO - doesn't do anything, need sqlite extetion to add encryption?

    ret = _db.open();

    QSqlQuery query;
    QString sql;
    //QSqlTableModel *model;
    GenericTableModel *model;

    if (ret) {
        // Create and/or migrate tables

        // ===========================================
        // Create student users table
        sql = "CREATE TABLE IF NOT EXISTS `users` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `name`          TEXT NOT NULL DEFAULT '', \
                `sortable_name` TEXT NOT NULL DEFAULT '', \
                `short_name`    TEXT NOT NULL DEFAULT '', \
                `permissions`   TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "users", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE `users` ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "users", _db);
        }

        // ===========================================
        // Create the courses table
        sql = "CREATE TABLE IF NOT EXISTS `courses` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `name`          TEXT NOT NULL DEFAULT '', \
                `account_id`    TEXT NOT NULL DEFAULT '', \
                `start_at`      TEXT NOT NULL DEFAULT '', \
                `grading_standard_id` TEXT NOT NULL DEFAULT '', \
                `is_public`     TEXT NOT NULL DEFAULT '', \
                `course_code`   TEXT NOT NULL DEFAULT '', \
                `default_view`  TEXT NOT NULL DEFAULT '', \
                `root_account_id` TEXT NOT NULL DEFAULT '', \
                `enrollment_term_id` TEXT NOT NULL DEFAULT '', \
                `end_at`        TEXT NOT NULL DEFAULT '', \
                `public_syllabus` TEXT NOT NULL DEFAULT '', \
                `public_syllabus_to_auth` TEXT NOT NULL DEFAULT '', \
                `storage_quota_mb` TEXT NOT NULL DEFAULT '', \
                `is_public_to_auth_users` TEXT NOT NULL DEFAULT '', \
                `apply_assignment_group_weights` TEXT NOT NULL DEFAULT '', \
                `calendar`       TEXT NOT NULL DEFAULT '', \
                `time_zone`      TEXT NOT NULL DEFAULT '', \
                `hide_final_grades` TEXT NOT NULL DEFAULT '', \
                `workflow_state` TEXT NOT NULL DEFAULT '', \
                `restrict_enrollments_to_course_dates` TEXT NOT NULL DEFAULT '', \
                `enrollment_type` TEXT NOT NULL DEFAULT '', \
                `enrollment_role` TEXT NOT NULL DEFAULT '', \
                `enrollment_role_id` TEXT NOT NULL DEFAULT '', \
                `enrollment_state` TEXT NOT NULL DEFAULT '', \
                `enrollment_user_id` TEXT NOT NULL DEFAULT '' \
                );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "courses", _db);

        // Modify table - add is_active field to mark courses that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE courses ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "courses", _db);
        }



        // ===========================================
        // Create modules  table
        sql = "CREATE TABLE IF NOT EXISTS `modules` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `name`          TEXT NOT NULL DEFAULT '', \
                `position`      TEXT NOT NULL DEFAULT '', \
                `unlock_at`     TEXT NOT NULL DEFAULT '' , \
                `require_sequential_progress` TEXT NOT NULL DEFAULT '', \
                `publish_final_grade` TEXT NOT NULL DEFAULT '', \
                `prerequisite_module_ids` TEXT NOT NULL DEFAULT '', \
                `published`     TEXT NOT NULL DEFAULT '', \
                `items_count`   TEXT NOT NULL DEFAULT '', \
                `items_url`     TEXT NOT NULL DEFAULT '', \
                `course_id`     TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "modules", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE modules ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "modules", _db);
        }


        // ===========================================
        // Create module_items table
        sql = "CREATE TABLE IF NOT EXISTS `module_items` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `title`          TEXT NOT NULL DEFAULT '', \
                `position`      TEXT NOT NULL DEFAULT '', \
                `indent`     TEXT NOT NULL DEFAULT '' , \
                `type` TEXT NOT NULL DEFAULT '', \
                `module_id` TEXT NOT NULL DEFAULT '', \
                `html_url` TEXT NOT NULL DEFAULT '', \
                `page_url`     TEXT NOT NULL DEFAULT '', \
                `url`   TEXT NOT NULL DEFAULT '', \
                `published`     TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "module_items", _db);

        // Modify table - add missing colunms
        if (model->getColumnIndex("content_id") == -1)  {
            // Column doesn't exit
            QStringList add_columns;
            add_columns << "ALTER TABLE module_items ADD COLUMN `content_id` TEXT NOT NULL DEFAULT ''";
            add_columns << "ALTER TABLE module_items ADD COLUMN `external_url` TEXT NOT NULL DEFAULT ''";
            add_columns << "ALTER TABLE module_items ADD COLUMN `new_tab`    TEXT NOT NULL DEFAULT ''";
            add_columns << "ALTER TABLE module_items ADD COLUMN `completion_requirement` TEXT NOT NULL DEFAULT ''";
            add_columns << "ALTER TABLE module_items ADD COLUMN `content_details` TEXT NOT NULL DEFAULT ''";
            qDebug() << "Adding columns to module_items to courses";
            foreach (sql, add_columns) {
                if (!query.exec(sql)) {
                    qDebug() << "DB Error: " << query.lastError().text();
                }
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "module_items", _db);
        }

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE module_items ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "module_items", _db);
        }


        // ===========================================
        // Create folders table - holds folders for files
        sql = "CREATE TABLE IF NOT EXISTS `folders` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `name`          TEXT NOT NULL DEFAULT '', \
                `full_name`     TEXT NOT NULL DEFAULT '', \
                `context_id`    TEXT NOT NULL DEFAULT '', \
                `context_type`  TEXT NOT NULL DEFAULT '', \
                `parent_folder_id` TEXT NOT NULL DEFAULT '', \
                `created_at`    TEXT NOT NULL DEFAULT '', \
                `updated_at`    TEXT NOT NULL DEFAULT '', \
                `lock_at`       TEXT NOT NULL DEFAULT '', \
                `unlock_at`     TEXT NOT NULL DEFAULT '', \
                `position`      TEXT NOT NULL DEFAULT '', \
                `locked`        TEXT NOT NULL DEFAULT '', \
                `folders_url`   TEXT NOT NULL DEFAULT '', \
                `files_url`     TEXT NOT NULL DEFAULT '', \
                `files_count`   TEXT NOT NULL DEFAULT '', \
                `folders_count` TEXT NOT NULL DEFAULT '', \
                `hidden`        TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `hidden_for_user` TEXT NOT NULL DEFAULT '', \
                `for_submissions` TEXT NOT NULL DEFAULT '' \
              );";

         if (!query.exec(sql)) {
             qDebug() << "DB Error: " << query.lastError().text();
             ret = false;
         }
         commit();

         // Add to the table models list
         model = new GenericTableModel(this, "folders", _db);

         // Modify table - add is_active field to mark items that are active
         if (model->getColumnIndex("is_active") == -1)  {
             // Column doesn't exit
             qDebug() << "Adding is_active column to courses";
             sql = "ALTER TABLE folders ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
             if (!query.exec(sql)) {
                 qDebug() << "DB Error: " << query.lastError().text();
             }
             commit();

             // Reload the model with the latest changes
             model = new GenericTableModel(this, "folders", _db);
         }

        // ===========================================
        // Create files table - holds file info
        sql = "CREATE TABLE IF NOT EXISTS `files` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `folder_id`     TEXT NOT NULL DEFAULT '', \
                `display_name`  TEXT NOT NULL DEFAULT '', \
                `filename`      TEXT NOT NULL DEFAULT '', \
                `content_type`  TEXT NOT NULL DEFAULT '', \
                `url`           TEXT NOT NULL DEFAULT '', \
                `size`          TEXT NOT NULL DEFAULT '', \
                `created_at`    TEXT NOT NULL DEFAULT '', \
                `updated_at`    TEXT NOT NULL DEFAULT '', \
                `unlock_at`     TEXT NOT NULL DEFAULT '', \
                `locked`        TEXT NOT NULL DEFAULT '', \
                `hidden`        TEXT NOT NULL DEFAULT '', \
                `lock_at`       TEXT NOT NULL DEFAULT '', \
                `hidden_for_user` TEXT NOT NULL DEFAULT '', \
                `thumbnail_url` TEXT NOT NULL DEFAULT '', \
                `modified_at`   TEXT NOT NULL DEFAULT '', \
                `mime_class`    TEXT NOT NULL DEFAULT '', \
                `media_entry_id` TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `course_id`     TEXT NOT NULL DEFAULT '', \
                `pull_file`      TEXT NOT NULL DEFAULT '', \
                `local_copy_present`    TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "files", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE files ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "files", _db);
        }


        // ===========================================
        // Create pages table
        sql = "CREATE TABLE IF NOT EXISTS `pages` ( \
                `title`         TEXT NOT NULL DEFAULT '', \
                `created_at`    TEXT NOT NULL DEFAULT '', \
                `url`           TEXT NOT NULL DEFAULT '', \
                `editing_roles` TEXT NOT NULL DEFAULT '', \
                `page_id`       TEXT NOT NULL DEFAULT '', \
                `published`     TEXT NOT NULL DEFAULT '', \
                `hide_from_students` TEXT NOT NULL DEFAULT '', \
                `front_page`    TEXT NOT NULL DEFAULT '', \
                `html_url`      TEXT NOT NULL DEFAULT '', \
                `updated_at`    TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `body`          TEXT NOT NULL DEFAULT '', \
                `lock_info`     TEXT NOT NULL DEFAULT '', \
                `lock_explanation` TEXT NOT NULL DEFAULT '', \
                `course_id`     TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "pages", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE pages ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "pages", _db);
        }



        // ===========================================
        // Create conversations table - messaging
        sql = "CREATE TABLE IF NOT EXISTS `conversations` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `subject`       TEXT NOT NULL DEFAULT '', \
                `workflow_state` TEXT NOT NULL DEFAULT '', \
                `last_message`  TEXT NOT NULL DEFAULT '', \
                `last_message_at` TEXT NOT NULL DEFAULT '', \
                `last_authored_message` TEXT NOT NULL DEFAULT '', \
                `last_authored_message_at` TEXT NOT NULL DEFAULT '', \
                `message_count` TEXT NOT NULL DEFAULT '', \
                `subscribed`    TEXT NOT NULL DEFAULT '', \
                `private`       TEXT NOT NULL DEFAULT '', \
                `starred`       TEXT NOT NULL DEFAULT '', \
                `properties`    TEXT NOT NULL DEFAULT '', \
                `audience`      TEXT NOT NULL DEFAULT '', \
                `audience_contexts` TEXT NOT NULL DEFAULT '', \
                `avatar_url`    TEXT NOT NULL DEFAULT '', \
                `participants`  TEXT NOT NULL DEFAULT '', \
                `visible`       TEXT NOT NULL DEFAULT '', \
                `context_code`  TEXT NOT NULL DEFAULT '', \
                `context_name`  TEXT NOT NULL DEFAULT '', \
                `submissions`   TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "conversations", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE conversations ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "conversations", _db);
        }


        // ===========================================
        // Create messages table
        sql = "CREATE TABLE IF NOT EXISTS `messages` ( \
                `id`            TEXT NOT NULL DEFAULT '', \
                `author_id`     TEXT NOT NULL DEFAULT '', \
                `created_at`    TEXT NOT NULL DEFAULT '', \
                `generated`     TEXT NOT NULL DEFAULT '', \
                `body`          TEXT NOT NULL DEFAULT '', \
                `forwarded_messages` TEXT NOT NULL DEFAULT '', \
                `attachments`   TEXT NOT NULL DEFAULT '', \
                `media_comment` TEXT NOT NULL DEFAULT '', \
                `participating_user_ids` TEXT NOT NULL DEFAULT '', \
                `conversation_id` TEXT NOT NULL DEFAULT '', \
                `scope`         TEXT NOT NULL DEFAULT '' \
                );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "messages", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE messages ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "messages", _db);
        }
        // Add flag to push this message if needed.
        if (model->getColumnIndex("need_to_push") == -1) {
            // Column doesn't exist
            sql = "ALTER TABLE messages ADD COLUMN `need_to_push` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            model = new GenericTableModel(this, "messages", _db);
        }

        // Create assignment_submissions table
        // assignment_type = file (upload), url, text, none, on_paper, external
        sql = "CREATE TABLE IF NOT EXISTS `assignment_submissions` ( \
                `assignment_type` TEXT NOT NULL DEFAULT 'file', \
                `submission_text` TEXT NOT NULL DEFAULT '', \
                `queue_url`     TEXT NOT NULL DEFAULT '', \
                `origin_url`    TEXT NOT NULL DEFAULT '', \
                `file_size`     TEXT NOT NULL DEFAULT '', \
                `assignment_id` TEXT NOT NULL DEFAULT '', \
                `course_id`     TEXT NOT NULL DEFAULT '', \
                `queued_on`     TEXT NOT NULL DEFAULT '', \
                `synced_on`     TEXT NOT NULL DEFAULT '' \
              );";

        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "assignment_submissions", _db);

        // Create the assignments table
        sql = "CREATE TABLE IF NOT EXISTS `assignments` ( \
                `id`    INTEGER PRIMARY KEY AUTOINCREMENT, \
                `description`   TEXT NOT NULL DEFAULT '', \
                `due_at`        TEXT NOT NULL DEFAULT '', \
                `unlock_at`     TEXT NOT NULL DEFAULT '', \
                `lock_at`       TEXT NOT NULL DEFAULT '', \
                `points_possible` TEXT NOT NULL DEFAULT '', \
                `grading_type` TEXT NOT NULL DEFAULT '', \
                `assignment_group_id` TEXT NOT NULL DEFAULT '', \
                `grading_standard_id` TEXT NOT NULL DEFAULT '', \
                `created_at`    TEXT NOT NULL DEFAULT '', \
                `updated_at`    TEXT NOT NULL DEFAULT '', \
                `peer_reviews`  TEXT NOT NULL DEFAULT '', \
                `automatic_peer_reviews` TEXT NOT NULL DEFAULT '', \
                `position`      TEXT NOT NULL DEFAULT '', \
                `grade_group_students_individually` TEXT NOT NULL DEFAULT '', \
                `anonymous_peer_reviews` TEXT NOT NULL DEFAULT '', \
                `group_category_id` TEXT NOT NULL DEFAULT '', \
                `post_to_sis`   TEXT NOT NULL DEFAULT '', \
                `moderated_grading` TEXT NOT NULL DEFAULT '', \
                `omit_from_final_grade` TEXT NOT NULL DEFAULT '', \
                `intra_group_peer_reviews` TEXT NOT NULL DEFAULT '', \
                `secure_params` TEXT NOT NULL DEFAULT '', \
                `course_id`     TEXT NOT NULL DEFAULT '', \
                `name`          TEXT NOT NULL DEFAULT '', \
                `submission_types` TEXT NOT NULL DEFAULT '', \
                `has_submitted_submissions` TEXT NOT NULL DEFAULT '', \
                `due_date_required` TEXT NOT NULL DEFAULT '', \
                `max_name_length` TEXT NOT NULL DEFAULT '', \
                `in_closed_grading_period` TEXT NOT NULL DEFAULT '', \
                `is_quiz_assignment` TEXT NOT NULL DEFAULT '', \
                `muted`         TEXT NOT NULL DEFAULT '', \
                `html_url`      TEXT NOT NULL DEFAULT '', \
                `has_overrides` TEXT NOT NULL DEFAULT '', \
                `needs_grading_count` TEXT NOT NULL DEFAULT '', \
                `integration_id` TEXT NOT NULL DEFAULT '', \
                `integration_data` TEXT NOT NULL DEFAULT '', \
                `published`     TEXT NOT NULL DEFAULT '', \
                `unpublishable` TEXT NOT NULL DEFAULT '', \
                `only_visible_to_overrides` TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `submissions_download_url` TEXT NOT NULL DEFAULT '' \
              );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        model = new GenericTableModel(this, "assignments", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE assignments ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "assignments", _db);
        }

        // Create Announcements table
        sql = "CREATE TABLE IF NOT EXISTS `announcements` ( \
                `id`        TEXT NOT NULL DEFAULT '', \
                `title`     TEXT NOT NULL DEFAULT '', \
                `message`   TEXT NOT NULL DEFAULT '', \
                `html_url`  TEXT NOT NULL DEFAULT '', \
                `posted_at` TEXT NOT NULL DEFAULT '', \
                `last_reply_at` TEXT NOT NULL DEFAULT '', \
                `require_initial_post`  TEXT NOT NULL DEFAULT '', \
                `user_can_see_posts`    TEXT NOT NULL DEFAULT '', \
                `discussion_subentry_count` TEXT NOT NULL DEFAULT '', \
                `read_state`    TEXT NOT NULL DEFAULT '', \
                `unread_count`  TEXT NOT NULL DEFAULT '', \
                `subscribed`    TEXT NOT NULL DEFAULT '', \
                `subscription_hold` TEXT NOT NULL DEFAULT '', \
                `assignment_id` TEXT NOT NULL DEFAULT '', \
                `delayed_post_at`   TEXT NOT NULL DEFAULT '', \
                `published`     TEXT NOT NULL DEFAULT '', \
                `lock_at`       TEXT NOT NULL DEFAULT '', \
                `locked`        TEXT NOT NULL DEFAULT '', \
                `pinned`        TEXT NOT NULL DEFAULT '', \
                `locked_for_user`   TEXT NOT NULL DEFAULT '', \
                `lock_info`     TEXT NOT NULL DEFAULT '', \
                `lock_explanation`  TEXT NOT NULL DEFAULT '', \
                `user_name`     TEXT NOT NULL DEFAULT '', \
                `topic_children`    TEXT NOT NULL DEFAULT '', \
                `group_topic_children`  TEXT NOT NULL DEFAULT '', \
                `root_topic_id`     TEXT NOT NULL DEFAULT '', \
                `podcast_url`       TEXT NOT NULL DEFAULT '', \
                `discussion_type`   TEXT NOT NULL DEFAULT '', \
                `group_category_id` TEXT NOT NULL DEFAULT '', \
                `attachments`       TEXT NOT NULL DEFAULT '', \
                `permissions`       TEXT NOT NULL DEFAULT '', \
                `allow_rating`      TEXT NOT NULL DEFAULT '', \
                `only_graders_can_rate` TEXT NOT NULL DEFAULT '', \
                `sort_by_rating`    TEXT NOT NULL DEFAULT '', \
                `context_code`      TEXT NOT NULL DEFAULT '', \
                `course_id`         TEXT NOT NULL DEFAULT '', \
                `active`            TEXT NOT NULL DEFAULT '' \
               );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "announcements", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE announcements ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "announcements", _db);
        }


        // Create the resources table
        sql = "CREATE TABLE IF NOT EXISTS `resources` ( \
                `id`	INTEGER PRIMARY KEY AUTOINCREMENT, \
                `resource_name`	TEXT NOT NULL DEFAULT '', \
                `resource_url`	TEXT NOT NULL DEFAULT '', \
                `resource_description`	TEXT NOT NULL DEFAULT '', \
                `sort_order`	INTEGER NOT NULL DEFAULT 0 \
            );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "resources", _db);

        // Modify table - add is_active field to mark items that are active
        if (model->getColumnIndex("is_active") == -1)  {
            // Column doesn't exit
            qDebug() << "Adding is_active column to courses";
            sql = "ALTER TABLE resources ADD COLUMN `is_active` TEXT NOT NULL DEFAULT ''";
            if (!query.exec(sql)) {
                qDebug() << "DB Error: " << query.lastError().text();
            }
            commit();

            // Reload the model with the latest changes
            model = new GenericTableModel(this, "resources", _db);
        }

        // Make sure the default resource entries are in place
        add_resource("Start Here", "/start_here/index.html", "Getting started...", 0);


        // Add table for SMC media/document queue
        // Create SMC Video Queue
        sql = "CREATE TABLE IF NOT EXISTS `smc_media_dl_queue2` ( \
                `id` INTEGER PRIMARY KEY AUTOINCREMENT, \
                `media_id` TEXT NOT NULL DEFAULT '', \
                `original_host` TEXT NOT NULL DEFAULT '', \
                `original_url` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Create SMC Document Queue
        sql = "CREATE TABLE IF NOT EXISTS `smc_document_dl_queue2` ( \
                `id` INTEGER PRIMARY KEY AUTOINCREMENT, \
                `document_id` TEXT NOT NULL DEFAULT '', \
                `original_host` TEXT NOT NULL DEFAULT '', \
                `original_url` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();


        // Add table for canvas file download queue
        sql = "CREATE TABLE IF NOT EXISTS `canvas_dl_queue` ( \
                `id` INTEGER PRIMARY KEY AUTOINCREMENT, \
                `file_id` TEXT NOT NULL DEFAULT '', \
                `course_id` TEXT NOT NULL DEFAULT '', \
                `original_host` TEXT NOT NULL DEFAULT '', \
                `original_url` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add table for disucssion topics
        sql = "CREATE TABLE IF NOT EXISTS `discussion_topics` ( \
                `id` TEXT NOT NULL DEFAULT '', \
                `course_id` TEXT NOT NULL DEFAULT '', \
                `title` TEXT NOT NULL DEFAULT '', \
                `message` TEXT NOT NULL DEFAULT '', \
                `html_url` TEXT NOT NULL DEFAULT '', \
                `posted_at` TEXT NOT NULL DEFAULT '', \
                `last_reply_at` TEXT NOT NULL DEFAULT '', \
                `require_initial_post` TEXT NOT NULL DEFAULT '', \
                `user_can_see_posts` TEXT NOT NULL DEFAULT '', \
                `discussion_subentry_count` TEXT NOT NULL DEFAULT '', \
                `read_state` TEXT NOT NULL DEFAULT '', \
                `unread_count` TEXT NOT NULL DEFAULT '', \
                `subscribed` TEXT NOT NULL DEFAULT '', \
                `subscription_hold` TEXT NOT NULL DEFAULT '', \
                `assignment_id` TEXT NOT NULL DEFAULT '', \
                `delayed_post_at` TEXT NOT NULL DEFAULT '', \
                `published` TEXT NOT NULL DEFAULT '', \
                `lock_at` TEXT NOT NULL DEFAULT '', \
                `locked` TEXT NOT NULL DEFAULT '', \
                `pinned` TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `lock_info` TEXT NOT NULL DEFAULT '', \
                `lock_explanation` TEXT NOT NULL DEFAULT '', \
                `user_name` TEXT NOT NULL DEFAULT '', \
                `topic_children` TEXT NOT NULL DEFAULT '', \
                `group_topic_children` TEXT NOT NULL DEFAULT '', \
                `root_topic_id` TEXT NOT NULL DEFAULT '', \
                `podcast_url` TEXT NOT NULL DEFAULT '', \
                `discussion_type` TEXT NOT NULL DEFAULT '', \
                `group_category_id` TEXT NOT NULL DEFAULT '', \
                `attachments` TEXT NOT NULL DEFAULT '', \
                `permissions` TEXT NOT NULL DEFAULT '', \
                `allow_rating` TEXT NOT NULL DEFAULT '', \
                `only_graders_can_rate` TEXT NOT NULL DEFAULT '', \
                `sort_by_rating` TEXT NOT NULL DEFAULT '', \
                `is_active` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "discussion_topics", _db);



        // Add table for Quizzes
        sql = "CREATE TABLE IF NOT EXISTS `quizzes` ( \
                `id` TEXT NOT NULL DEFAULT '', \
                `course_id` TEXT NOT NULL DEFAULT '', \
                `title` TEXT NOT NULL DEFAULT '', \
                `html_url` TEXT NOT NULL DEFAULT '', \
                `mobile_url` TEXT NOT NULL DEFAULT '', \
                `preview_url` TEXT NOT NULL DEFAULT '', \
                `description` TEXT NOT NULL DEFAULT '', \
                `quiz_type` TEXT NOT NULL DEFAULT '', \
                `assignment_group_id` TEXT NOT NULL DEFAULT '', \
                `time_limit` TEXT NOT NULL DEFAULT '', \
                `shuffle_answers` TEXT NOT NULL DEFAULT '', \
                `hide_results` TEXT NOT NULL DEFAULT '', \
                `show_correct_answers` TEXT NOT NULL DEFAULT '', \
                `show_correct_answers_last_attempt` TEXT NOT NULL DEFAULT '', \
                `show_correct_answers_at` TEXT NOT NULL DEFAULT '', \
                `hide_correct_answers_at` TEXT NOT NULL DEFAULT '', \
                `one_time_results` TEXT NOT NULL DEFAULT '', \
                `scoring_policy` TEXT NOT NULL DEFAULT '', \
                `allowed_attempts` TEXT NOT NULL DEFAULT '', \
                `one_question_at_a_time` TEXT NOT NULL DEFAULT '', \
                `question_count` TEXT NOT NULL DEFAULT '', \
                `points_possible` TEXT NOT NULL DEFAULT '', \
                `cant_go_back` TEXT NOT NULL DEFAULT '', \
                `has_access_code` TEXT NOT NULL DEFAULT '', \
                `access_code` TEXT NOT NULL DEFAULT '', \
                `ip_filter` TEXT NOT NULL DEFAULT '', \
                `due_at` TEXT NOT NULL DEFAULT '', \
                `lock_at` TEXT NOT NULL DEFAULT '', \
                `unlock_at` TEXT NOT NULL DEFAULT '', \
                `published` TEXT NOT NULL DEFAULT '', \
                `unpublishable` TEXT NOT NULL DEFAULT '', \
                `locked_for_user` TEXT NOT NULL DEFAULT '', \
                `lock_info` TEXT NOT NULL DEFAULT '', \
                `lock_explanation` TEXT NOT NULL DEFAULT '', \
                `speedgrader_url` TEXT NOT NULL DEFAULT '', \
                `quiz_extensions_url` TEXT NOT NULL DEFAULT '', \
                `permissions` TEXT NOT NULL DEFAULT '', \
                `all_dates` TEXT NOT NULL DEFAULT '', \
                `version_number` TEXT NOT NULL DEFAULT '', \
                `question_types` TEXT NOT NULL DEFAULT '', \
                `anonymous_submissions` TEXT NOT NULL DEFAULT '', \
                `is_active` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "quizzes", _db);



        // Add table for Quiz questions
        sql = "CREATE TABLE IF NOT EXISTS `quiz_questions` ( \
                `id` TEXT NOT NULL DEFAULT '', \
                `course_id` TEXT NOT NULL DEFAULT '', \
                `quiz_id` TEXT NOT NULL DEFAULT '', \
                `quiz_group_id` TEXT NOT NULL DEFAULT '', \
                `assessment_question_id` TEXT NOT NULL DEFAULT '', \
                `position` TEXT NOT NULL DEFAULT '', \
                `question_type` TEXT NOT NULL DEFAULT '', \
                `payload_token` TEXT NOT NULL DEFAULT '', \
                `question_payload` TEXT NOT NULL DEFAULT '', \
                `is_active` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "quiz_questions", _db);



        // Add table for Quiz submissions
        sql = "CREATE TABLE IF NOT EXISTS `quiz_submissions` ( \
                `id` TEXT NOT NULL DEFAULT '', \
                `quiz_id` TEXT NOT NULL DEFAULT '', \
                `user_id` TEXT NOT NULL DEFAULT '', \
                `access_code` TEXT NOT NULL DEFAULT '', \
                `validation_token` TEXT NOT NULL DEFAULT '', \
                `submission_id` TEXT NOT NULL DEFAULT '', \
                `started_at` TEXT NOT NULL DEFAULT '', \
                `finished_at` TEXT NOT NULL DEFAULT '', \
                `end_at` TEXT NOT NULL DEFAULT '', \
                `attempt` TEXT NOT NULL DEFAULT '', \
                `extra_attempts` TEXT NOT NULL DEFAULT '', \
                `extra_time` TEXT NOT NULL DEFAULT '', \
                `manually_locked` TEXT NOT NULL DEFAULT '', \
                `time_spent` TEXT NOT NULL DEFAULT '', \
                `score` TEXT NOT NULL DEFAULT '', \
                `score_before_regrade` TEXT NOT NULL DEFAULT '', \
                `kept_score` TEXT NOT NULL DEFAULT '', \
                `fudge_points` TEXT NOT NULL DEFAULT '', \
                `has_seen_results` TEXT NOT NULL DEFAULT '', \
                `workflow_state` TEXT NOT NULL DEFAULT '', \
                `overdue_and_needs_submission` TEXT NOT NULL DEFAULT '', \
                `ready_to_submit` TEXT NOT NULL DEFAULT '', \
                `is_active` TEXT NOT NULL DEFAULT '' \
                );";
        if (!query.exec(sql)) {
           qDebug() << "DB Error: " << query.lastError().text();
           ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "quiz_submissions", _db);


        // Add table for Quiz question submissions
        sql = "CREATE TABLE IF NOT EXISTS `quiz_questions_submissions` ( \
                `id` TEXT NOT NULL DEFAULT '', \
                `quiz_submission_id` TEXT NOT NULL DEFAULT '', \
                `course_id` TEXT NOT NULL DEFAULT '', \
                `quiz_id` TEXT NOT NULL DEFAULT '', \
                `flagged` TEXT NOT NULL DEFAULT '', \
                `answer_txt` TEXT NOT NULL DEFAULT '', \
                `question_name` TEXT NOT NULL DEFAULT '', \
                `question_type` TEXT NOT NULL DEFAULT '', \
                `question_payload` TEXT NOT NULL DEFAULT '', \
                `needs_submitted` TEXT NOT NULL DEFAULT '', \
                `is_active` TEXT NOT NULL DEFAULT '' \
             );";
        if (!query.exec(sql)) {
            qDebug() << "DB Error: " << query.lastError().text();
            ret = false;
        }
        commit();

        // Add to the table models list
        model = new GenericTableModel(this, "quiz_questions_submissions", _db);



        // ==== Now create views ====

        // Query to join modules and module_items
        sql = "SELECT module_items.*, modules.name, modules.course_id, (modules.position) as sort_order1, (module_items.position) as sort_order2  FROM module_items, modules WHERE modules.id=module_items.module_id ";
        GenericQueryModel *view = new GenericQueryModel(this, "module_items", sql, _db);

        // Query to join folders and files
        sql = "SELECT files.*, folders.name, (folders.name || \"_\" || files.display_name) as sort_order FROM folders, files WHERE folders.id=files.folder_id";
        view = new GenericQueryModel(this, "file_folders", sql, _db);

    }

    //qDebug() << "DB INIT complete " << this;
    return ret;
}

/**
 * @brief APP_DB::auth_student - Authenticate the student against the local database
 * @param user_name
 * @param password
 * @return true on success
 */
bool APP_DB::auth_student(QString user_name, QString password)
{
    bool ret = false;

    // If the password is empty always return false
    if (password == "") {
        return false;
    }

    // Encode password
    QString crypt_pw = QCryptographicHash::hash(password.toLocal8Bit(),
            QCryptographicHash::Sha256).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

    // Query to see if the user exists
    QString sql = "SELECT COUNT(*) user_count FROM students WHERE user_name=:user_name and password=:password";
    QSqlQuery q;
    q.prepare(sql);
    q.bindValue(":user_name", user_name);
    q.bindValue(":password", crypt_pw);
    q.exec();
    int user_count = 0;
    while(q.next()) {
        user_count = q.value("user_count").toInt();
    }

    if (user_count > 0) {
        ret = true;
    }

    return ret;
}

bool APP_DB::add_resource(QString resource_name, QString resource_url, QString resource_description, int sort_order)
{
    bool ret = false;
    QString sql;
    QSqlQuery q;

    // Check if the resource exists
    sql = "SELECT count(*) as count FROM resources WHERE resource_name=:resource_name";
    q.prepare(sql);
    q.bindValue(":resource_name", resource_name);
    q.exec();
    int count = 0;
    while (q.next()) {
        count = q.value("count").toInt();
    }

    if (count > 0) {
        // Already present
        return true;
    }

    // Not present, add it
    sql = "INSERT INTO resources (resource_name, resource_url, resource_description, sort_order) \
        VALUES (:resource_name, :resource_url, :resource_description, :sort_order)";


    q.prepare(sql);
    q.bindValue(":resource_name", resource_name);
    q.bindValue(":resource_url", resource_url);
    q.bindValue(":resource_description", resource_description);
    q.bindValue(":sort_order", sort_order);
    q.exec();

    // Write changes to the db
    commit();

    // Refresh data after insert? TODO - should this be an emit signal instead?
    //_tables["resources"]->select();

    ret = true;

    return ret;
}

GenericTableModel *APP_DB::getTable(QString table_name)
{
    //qDebug() << "getTable " << this;
    if (_tables.isEmpty())
        return nullptr;

    if (_tables.contains(table_name)) {
        return _tables[table_name];
    }

    return nullptr;
}

bool APP_DB::commit()
{
    return _db.commit();
}

bool APP_DB::rollback()
{
    return _db.rollback();
}

GenericTableModel::GenericTableModel(APP_DB *parent, QString table_name, QSqlDatabase db) : QSqlTableModel(parent, db)
{
    setTable(table_name);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    if (parent != nullptr) {
        parent->_tables[table_name] = this;

        // Expose this to QML
        //QQmlEngine *p = qobject_cast<QQmlApplicationEngine*>(this->parent()->parent());
        if (APP_DB::_engine == nullptr) {
            // Unable to get app engine
            qDebug() << "FATAL ERROR - Generic Table Model Unable to get the QML Application Engine.";
            QCoreApplication::quit();
            return;
        }
        APP_DB::_engine->rootContext()->setContextProperty(table_name + "_model", this);
    }
}

void GenericTableModel::setTable(const QString &tableName)
{
    QSqlTableModel::setTable(tableName);
    generateRoleNames();
}

QVariant GenericTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIndex = role - Qt::UserRole -1;
        QModelIndex modelIndex = this->index(index.row(), columnIndex);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }

    return value;
}

void GenericTableModel::modifyFilter(QString f)
{
    // Expose a method to QML that can set the filter for this table
    setFilter(f);
}

QHash<QString, QVariant> GenericTableModel::getRecord(int row) const
{
    QSqlRecord r = record(row);
    QHash<QString, QVariant> rhash;

    for (int i = 0; i < r.count(); i++) {
        rhash[r.fieldName(i)] = r.value(i);
    }

    return rhash;
}

bool GenericTableModel::newRecord(const QVariantMap &new_record)
{
    // Generate insert command (NOTE - DON'T use record interface - it sucks!)
    QString field_names = "";
    QString field_values = "";
    QSqlQuery query(database());

    QList<QString> bound_keys;

    // Add the field names
    foreach(QString field_name, new_record.keys()) {
        if (field_names != "") {
            field_names += ", ";
        }
        field_names += "`" + field_name + "`";

        // Add the place holder
        if(field_values != "") {
            field_values += ", ";
        }
        field_values += ":" + field_name.replace(" ", "_");

        // Add to the list to bind later (Can't bind before prepare)
        bound_keys.append(field_name);
    }


    QString sql = "INSERT INTO `" + tableName() + "` (" + field_names +
            ") VALUES (" + field_values + ");";
    query.prepare(sql);

    // Bind values - Do this w a QList to keep same order
    for(int i=0; i< bound_keys.length(); i++) {
        QString field_name = bound_keys[i];
        // NOTE - Javascript empty strings need to be converted this way to not be "null"
        // when bound to sql statement.
        QString field_value = new_record[field_name].toByteArray();
        //if (field_value == "") { field_value = ""; }

        //qDebug() << "Binding Field Value: " << field_value;

        query.bindValue(":" + field_name.replace(" ", "_"), field_value);

    }
    //query.bindValue(":forwarded_messages", "late_bind");

    // Start transaction
//    if (!database().transaction()) {
//        // Unable to start transaction?
//        qDebug() << "newRecord:" << tableName() << "ERROR - Unable to start transaction! "
//                 << query.lastError().text();
//        return false;
//    }

    if (!query.exec()) {
        qDebug() << "newRecord:" << tableName() << "ERROR - Couldn't save new record! "
                 << query.lastError().text() << query.lastQuery() << query.executedQuery()
                 << query.boundValues()
                 << query.boundValue(":forwarded_messages")
                 << query.boundValue(":need_to_push")
                 << database().hostName();
        query.finish();
        //database().rollback();
        return false;
    }
    query.finish();
    database().commit();

    return true;
}

QString GenericTableModel::getColumnName(int col_index)
{
    // Return the column for this index
    QString ret = "";

    QSqlRecord r = record();
    QSqlField f = r.field(col_index);
    ret = f.name();

    return ret;
}

int GenericTableModel::getColumnIndex(QString col_name)
{
    // Find the index for this column
    int ret = -1;

    ret = fieldIndex(col_name);

    return ret;
}

void GenericTableModel::sortOn(QString col_name, Qt::SortOrder order)
{
    // Lookup column index
    int col_index = getColumnIndex(col_name);
    sort(col_index, order);
}

GenericTableModel *GenericTableModel::copy()
{
    // Generate a copy of this table model
    GenericTableModel *m = new GenericTableModel(nullptr, this->tableName(), database());
    m->setFilter(this->filter());

    return m;
}

void GenericTableModel::generateRoleNames()
{
    m_roleNames.clear();
    for(int i = 0; i < record().count(); i++) {
        m_roleNames.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
}


GenericQueryModel::GenericQueryModel(APP_DB *parent, QString query_name,
                                     QString query, QSqlDatabase /*db*/)
{
    m_sort_column = "";
    m_sort_type = Qt::AscendingOrder;
    m_filter = "";
    m_combine_filter_w_and = true;
    m_sql_select = query;
    m_query_name = query_name;
    setCombinedQuery();
    if (parent != nullptr) {
        parent->_queries[query_name] = this;

        // Expose this to QML
        //QQmlEngine *p = qobject_cast<QQmlApplicationEngine*>(this->parent()->parent());
        if (APP_DB::_engine == nullptr) {
            // Unable to get app engine
            qDebug() << "FATAL ERROR - Generic Query Model Unable to get the QML Application Engine.";
            QCoreApplication::quit();
            return;
        }
        APP_DB::_engine->rootContext()->setContextProperty(query_name + "_query", this);
    }
}

void GenericQueryModel::refresh()
{
    setCombinedQuery();
}

QVariant GenericQueryModel::data(const QModelIndex &index, int role) const
{
    // Return the requested data
    QVariant value = QSqlQueryModel::data(index, role);
    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIndex= role - Qt::UserRole- 1;
        QModelIndex modelIndex = this->index(index.row(), columnIndex);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

void GenericQueryModel::modifyQuery(QString q)
{
    // Change the query for this model
    m_sql_select = q;

    // Calculate and reset the query
    setCombinedQuery();
}

void GenericQueryModel::modifyFilter(QString f, bool combine_w_and)
{
    // Adjust the filter portion of the sql statement
    m_filter = f;
    m_combine_filter_w_and = combine_w_and;
    // Calculate and reset the query
    setCombinedQuery();
}

QHash<QString, QVariant> GenericQueryModel::getRecord(int row) const
{
    QSqlRecord r = record(row);
    QHash<QString, QVariant> rhash;

    for (int i = 0; i < r.count(); i++) {
        rhash[r.fieldName(i)] = r.value(i);
    }

    return rhash;
}

QString GenericQueryModel::getColumnName(int col_index)
{
    // Return the column for this index
    QString ret = "";

    QSqlRecord r = record();
    QSqlField f = r.field(col_index);
    ret = f.name();

    return ret;
}

int GenericQueryModel::getColumnIndex(QString col_name)
{
    // Find the index for this column
    int ret = -1;

    // this doesn't exist for qsqlquerymodel - so we do our own
    //ret = fieldIndex(col_name);
    for(int i = 0; i < record().count(); i++) {
        if (record().fieldName(i).toUtf8() == col_name) {
            // Return the column index, not the role number
            ret = i; // + Qt::UserRole + 1;
            break;
        }
    }
    return ret;
}

void GenericQueryModel::sortOn(QString col_name, Qt::SortOrder order)
{
    m_sort_column = col_name;
    m_sort_type = order;
    setCombinedQuery();
}

void GenericQueryModel::generateRoleNames()
{
    m_roleNames.clear();
    for(int i = 0; i < record().count(); i++) {
        m_roleNames.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
}

void GenericQueryModel::setCombinedQuery()
{
    // Query is a combination of query and filter. Combine them and set it as the
    // current query.
    QString q = "";
    q = m_sql_select;

    // Add the filter if there is something to add
    if (m_filter != "") {
        // Need to combine, see if we need to add WHERE or if it is already present
        if (q.contains("WHERE", Qt::CaseInsensitive)) {
            if (m_combine_filter_w_and) {
                q += " AND ";
            } else {
                q += " OR ";
            }
            q += m_filter;
        } else {
            // No where clause, append it with the filter
            q += " WHERE " + m_filter;
        }
    }
    if (m_sort_column != "") {
        q += " ORDER BY " + m_sort_column;
        if (m_sort_type == Qt::AscendingOrder) {
            q += " ASC";
        } else {
            q += " DESC";
        }
    }
    // qDebug() << "Set filter: " << q;

    setQuery(q);
    generateRoleNames();
}
