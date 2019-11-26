-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Nov 06, 2019 at 09:39 PM
-- Server version: 5.7.27-0ubuntu0.18.04.1
-- PHP Version: 7.2.24-0ubuntu0.18.04.1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `itri_forging`
--

-- --------------------------------------------------------

--
-- Table structure for table `CustomSenorSN1`
--

CREATE TABLE `CustomSenorSN1` (
  `ShotSN` bigint(20) UNSIGNED NOT NULL,
  `MaxPressure` double DEFAULT NULL,
  `SumStage1Pressure` double DEFAULT NULL,
  `MaxPressureTime` double DEFAULT NULL,
  `AvgStage1PressureSlope` double DEFAULT NULL,
  `AvgStage2PressureSlope` double DEFAULT NULL,
  `MoldReleasePressure` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `CustomSenorSN1_TableSN1`
--

CREATE TABLE `CustomSenorSN1_TableSN1` (
  `ShotSN` bigint(20) UNSIGNED NOT NULL,
  `ElapsedTime` double UNSIGNED DEFAULT NULL,
  `SensorValue` double DEFAULT NULL,
  `DisplayValue` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `CustomSenorSN2`
--

CREATE TABLE `CustomSenorSN2` (
  `ShotSn` bigint(20) UNSIGNED NOT NULL,
  `MaxPressure` double DEFAULT NULL,
  `SumStage1Pressure` double DEFAULT NULL,
  `MaxPressureTime` double DEFAULT NULL,
  `AvgStage1PressureSlope` double DEFAULT NULL,
  `AvgStage2PressureSlope` double DEFAULT NULL,
  `MoldReleasePressure` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `CustomSenorSN3`
--

CREATE TABLE `CustomSenorSN3` (
  `ShotSN` bigint(20) UNSIGNED NOT NULL,
  `MaxPressure` double DEFAULT NULL,
  `SumStage1Pressure` double DEFAULT NULL,
  `MaxPressureTime` double DEFAULT NULL,
  `AvgStage1PressureSlope` double DEFAULT NULL,
  `AvgStage2PressureSlope` double DEFAULT NULL,
  `MoldReleasePressure` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `CustomSensorSNList`
--

CREATE TABLE `CustomSensorSNList` (
  `CustomSensorSN` int(10) UNSIGNED NOT NULL,
  `CustomSensorClass` int(10) UNSIGNED NOT NULL,
  `CustomSensorModelSn` int(10) UNSIGNED NOT NULL,
  `CustomSenorChannelSN` int(10) UNSIGNED NOT NULL,
  `CustomSenorCovertRatio` double NOT NULL,
  `CustomSenorMeta` varchar(250) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `CustomSensorSNList`
--

INSERT INTO `CustomSensorSNList` (`CustomSensorSN`, `CustomSensorClass`, `CustomSensorModelSn`, `CustomSenorChannelSN`, `CustomSenorCovertRatio`, `CustomSenorMeta`) VALUES
(1, 2, 3, 1, 1, NULL),
(2, 2, 3, 2, 1, NULL),
(3, 2, 3, 3, 1, NULL),
(5, 4, 4, 4, 5, NULL);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `CustomSenorSN1_TableSN1`
--
ALTER TABLE `CustomSenorSN1_TableSN1`
  ADD KEY `ShotSN` (`ShotSN`);

--
-- Indexes for table `CustomSensorSNList`
--
ALTER TABLE `CustomSensorSNList`
  ADD PRIMARY KEY (`CustomSensorSN`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
