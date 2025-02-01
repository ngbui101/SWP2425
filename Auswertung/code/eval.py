
import read
import numpy as np
import pandas as pd

#calculate the error
def haversine(lat1, lon1, lat2, lon2):
    # Convert latitude and longitude from degrees to radians
    lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2])
    # Haversine formula
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    a = np.sin(dlat / 2)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon / 2)**2
    c = 2 * np.arcsin(np.sqrt(a))
    R = 6371000
    # Distance in meters
    distance = R * c
    return distance

def calc_error_gsm_lte_stationary(gt_df, data_df):

    errors = []
    gt_lat = gt_df.loc[0]['Latitude']
    gt_lon = gt_df.loc[0]['Longitude']
    #print(gt_df.loc[0]['Latitude'])
    #print(gt_lat, " ", gt_lon)
    for i in range(len(data_df)):
        #print(data_df.loc[i]['Latitude'], data_df.loc[i]['Longitude'] )
        
        data_df_lat = data_df.loc[i]['Latitude']
        data_df_lon = data_df.loc[i]['Longitude']
        #print("GT: ", gt_lat, " ", gt_lon)
        #print("DT ", data_df_lat, " ", data_df_lon)
        #print("Error: ", haversine(gt_lat, gt_lon, data_df_lat, data_df_lon))
        
        errors.append(haversine(gt_lat, gt_lon, data_df_lat, data_df_lon))
    #print(errors)
    return errors

def calc_error_gsm_lte_moving(data_df, gt_df):
    errors = []
    pairs = find_nearest_groundtruth_pairs(data_df, gt_df)
    for (pos1,pos2) in pairs:
        data_df_lat, data_df_lon = pos1
        gt_lat, gt_lon = pos2
        #print("GT: ", gt_lat, " ", gt_lon)
        #print("DT ", data_df_lat, " ", data_df_lon)
        #print("Error: ", haversine(gt_lat, gt_lon, data_df_lat, data_df_lon))
        errors.append(haversine(data_df_lat, data_df_lon, gt_lat, gt_lon))
    return errors

def find_nearest_groundtruth_pairs(mess_df, groundtruth_df):
    """
    Findet für jede Messung die Groundtruth-Position mit dem nächsten Zeitstempel.
    
    :param mess_df: DataFrame mit Messdaten (enthält 'Latitude', 'Longitude', 'Timestamp').
    :param groundtruth_df: DataFrame mit Groundtruth-Daten (enthält 'Latitude', 'Longitude', 'Timestamp').
    :return: Liste von Tupeln, jeweils ((Mess_Lat, Mess_Lon), (Groundtruth_Lat, Groundtruth_Lon)).
    """

    mess_df['Timestamp'] = pd.to_datetime(mess_df['Timestamp'])
    groundtruth_df['Timestamp'] = pd.to_datetime(groundtruth_df['Timestamp'])
    # Liste zur Speicherung der Paare
    measurement_groundtruth_pairs = []

    # Für jede Messung den Groundtruth-Punkt mit dem nächsten Zeitstempel finden
    for _, row in mess_df.iterrows():
        # Berechne die absoluten Zeitunterschiede
        time_diffs = (groundtruth_df['Timestamp'] - row['Timestamp']).abs()
        min_index = time_diffs.idxmin()  # Index der minimalen Differenz
        
        # Erstelle ein Tupel: ((Mess_Lat, Mess_Lon), (Groundtruth_Lat, Groundtruth_Lon))
        pair = (
            (row['Latitude'], row['Longitude']),
            (groundtruth_df.iloc[min_index]['Latitude'], groundtruth_df.iloc[min_index]['Longitude'])
        )
        print()
        print("timestamp dt", row['Timestamp'])
        
        print("Timestamp gt", groundtruth_df.iloc[min_index]['Timestamp'])
        print((row['Latitude'], row['Longitude']))
        print((groundtruth_df.iloc[min_index]['Latitude'], groundtruth_df.iloc[min_index]['Longitude']))
        measurement_groundtruth_pairs.append(pair)
    
    return measurement_groundtruth_pairs
#Gets every gps error
def get_gps_errors(data_df):
    errors = []
    for i in range(len(data_df)):
        #print(data_df.loc[i]['Error'])
        errors.append(data_df.loc[i]['Error'])
    return(errors)


