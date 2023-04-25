import { collection, onSnapshot, query } from 'firebase/firestore';
import { useEffect, useMemo, useState } from 'react';
import { AiOutlineScan } from 'react-icons/ai';
import { FaWeightHanging } from 'react-icons/fa';
import { GiLaserBurst } from 'react-icons/gi';
import { toast } from 'react-toastify';
import { db } from '../utils/firebaseSetup';

function StatusBox({ flag, icon, sensorName, unlocked }) {
    // console.log('rendering', flag, unlocked);
    return (
        <div className='flex flex-col gap-4 items-center bg-white rounded-md border-2 p-4 w-[300px]'>
            <div className='svg'>{icon}</div>
            <p className='text-step-2'>{sensorName}</p>
            {unlocked === undefined ? (
                <div className='bg-red-400 text-white font-bold px-4 py-2 rounded-full'>
                    Loading...
                </div>
            ) : unlocked === 'false' ? (
                <div className='bg-gray-400 text-white font-bold px-4 py-2 rounded-full'>
                    Unlocked
                </div>
            ) : flag === 'true' ? (
                <div className='bg-green-400 text-white font-bold px-4 py-2 rounded-full'>
                    Safe
                </div>
            ) : (
                <div className='bg-red-400 text-white font-bold px-4 py-2 rounded-full'>
                    Danger
                </div>
            )}
        </div>
    );
}

function Home() {
    const [loading, setLoading] = useState(false);
    const [data, setData] = useState(undefined);

    useEffect(() => {
        const unsub = onSnapshot(
            query(collection(db, 'sensordata')),
            (snapshot) => {
                const data = snapshot.docs.sort((a, b) => {
                    return (
                        new Date(b.data().timestamp) -
                        new Date(a.data().timestamp)
                    );
                });
                setLoading(true);
                setData(data.map((doc) => doc.data()));
                // console.log(data[0].data());
                setLoading(false);
            }
        );
        return () => {
            unsub();
        };
    }, []);

    // useEffect(() => {
    //     const latest = data ? data[0] : undefined;
    //     if (latest) {
    //         if (
    //             (latest.rfid || latest.force || latest.laser) &&
    //             latest.unlocked
    //         ) {
    //             toast('DANGER!!', { type: 'error' });
    //         }
    //     }
    // }, [data]);

    const rfidFlag = useMemo(
        () => (data !== undefined ? data[0]?.rfid : undefined),
        [data]
    );
    const laserFlag = useMemo(
        () => (data !== undefined ? data[0]?.laser : undefined),
        [data]
    );
    const forceFlag = useMemo(
        () => (data !== undefined ? data[0]?.force : undefined),
        [data]
    );
    const unlocked = useMemo(
        () => (data !== undefined ? data[0]?.unlocked : undefined),
        [data]
    );

    return (
        <div className='min-h-screen bg-slate-50 text-slate-900 p-8'>
            <h1 className='text-step-6 text-center font-bold'>FWL PP V1</h1>
            <div className='flex flex-row flex-wrap gap-8 justify-center items-center py-10'>
                <StatusBox
                    icon={<AiOutlineScan />}
                    flag={rfidFlag}
                    unlocked={unlocked}
                    sensorName='RFID'
                />
                <StatusBox
                    icon={<GiLaserBurst />}
                    flag={laserFlag}
                    unlocked={unlocked}
                    sensorName='LASER'
                />
                <StatusBox
                    icon={<FaWeightHanging />}
                    flag={forceFlag}
                    unlocked={unlocked}
                    sensorName='PRESSURE'
                />
            </div>
            <div className='flex flex-row flex-wrap gap-4'>
                {data?.map((doc, index) => (
                    <div
                        key={'doc' + index}
                        className='p-4 rounded-md bg-white shadow-md'>
                        <p>RFID : {String(doc.rfid)}</p>
                        <p>Laser : {String(doc.laser)}</p>
                        <p>Force Sensor : {String(doc.force)}</p>
                        <p key={'doc' + index}>
                            {new Date(doc.timestamp).toLocaleString()}
                        </p>
                    </div>
                ))}
            </div>
        </div>
    );
}

export default Home;
