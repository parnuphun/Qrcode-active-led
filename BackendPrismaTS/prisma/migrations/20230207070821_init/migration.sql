-- CreateTable
CREATE TABLE `Iot_Lab_Led` (
    `Iot_Lab_Led_id` INTEGER NOT NULL AUTO_INCREMENT,
    `Iot_Lab_Led_Status` BOOLEAN NOT NULL,

    PRIMARY KEY (`Iot_Lab_Led_id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
