DELETE FROM `server_string` WHERE `entry` IN(2027,2028);
INSERT INTO `server_string` (`entry`, `content_default`) VALUES
('2027', 'There are pending open tickets, please close them first!'),
('2028', 'All closed tickets were deleted and counter is reseted to |cffff00ff 1|r');